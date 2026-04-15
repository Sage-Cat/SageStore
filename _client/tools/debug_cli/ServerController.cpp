#include "ServerController.hpp"

#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QTcpServer>
#include <QTimer>
#include <QUrl>
#include <QThread>

#include <algorithm>

#include "JsonUtils.hpp"
#include "common/Endpoints.hpp"
#include "common/Keys.hpp"
#include "common/Network/JsonSerializer.hpp"

namespace DebugCli {
namespace {
QString datasetErrorMessage(const Dataset &dataset)
{
    const auto it = dataset.find(Keys::_ERROR);
    if (it == dataset.end()) {
        return {};
    }

    QStringList values;
    for (const std::string &value : it->second) {
        values.append(QString::fromStdString(value));
    }
    return values.join(QStringLiteral(" | "));
}

QByteArray serializeDataset(const Dataset &dataset)
{
    JsonSerializer serializer;
    return QByteArray::fromStdString(serializer.serialize(dataset));
}

Dataset deserializeDataset(const QByteArray &payload)
{
    JsonSerializer serializer;
    return serializer.deserialize(payload.toStdString());
}
} // namespace

ServerController::ServerController() = default;

ServerController::~ServerController() { stop(); }

ServerController::Config ServerController::configFromJson(const QJsonObject &object)
{
    Config config;
    if (object.contains(QStringLiteral("mode"))) {
        config.mode = object.value(QStringLiteral("mode")).toString(config.mode);
    }
    if (object.contains(QStringLiteral("scheme"))) {
        config.scheme = object.value(QStringLiteral("scheme")).toString(config.scheme);
    }
    if (object.contains(QStringLiteral("address"))) {
        config.address = object.value(QStringLiteral("address")).toString(config.address);
    }
    if (object.contains(QStringLiteral("port"))) {
        config.port = object.value(QStringLiteral("port")).toInt(config.port);
    }
    config.binaryPath = object.value(QStringLiteral("binaryPath")).toString();
    config.sqlPath = object.value(QStringLiteral("sqlPath")).toString();
    config.dbPath = object.value(QStringLiteral("dbPath")).toString();
    config.startupTimeoutMs =
        object.value(QStringLiteral("startupTimeoutMs")).toInt(config.startupTimeoutMs);
    return config;
}

OperationResult ServerController::prepare(const Config &inputConfig)
{
    m_config = normalizedConfig(inputConfig);

    if (m_config.mode.compare(QStringLiteral("spawn"), Qt::CaseInsensitive) != 0) {
        m_effectiveConfig = {.scheme = m_config.scheme.toStdString(),
                             .address = m_config.address.toStdString(),
                             .port = m_config.port};
        m_prepared = true;
        return OperationResult::success(state());
    }

    if (m_process.state() == QProcess::Running) {
        m_prepared = true;
        return OperationResult::success(state());
    }

    m_tempDir = std::make_unique<QTemporaryDir>();
    if (!m_tempDir->isValid()) {
        return OperationResult::failure(
            QStringLiteral("infra"),
            QStringLiteral("Unable to create a temporary server directory"));
    }

    const QString binaryPath = m_config.binaryPath.isEmpty()
                                   ? QStringLiteral(SAGESTORE_DEBUGCLI_DEFAULT_SERVER_BIN)
                                   : m_config.binaryPath;
    const QString sqlPath = m_config.sqlPath.isEmpty()
                                ? QStringLiteral(SAGESTORE_DEBUGCLI_DEFAULT_SERVER_SQL)
                                : m_config.sqlPath;
    if (!QFile::exists(binaryPath)) {
        return OperationResult::failure(
            QStringLiteral("infra"),
            QStringLiteral("Server binary not found: %1").arg(binaryPath),
            QJsonObject{{QStringLiteral("binaryPath"), binaryPath}});
    }
    if (!QFile::exists(sqlPath)) {
        return OperationResult::failure(
            QStringLiteral("infra"),
            QStringLiteral("Server SQL bootstrap not found: %1").arg(sqlPath),
            QJsonObject{{QStringLiteral("sqlPath"), sqlPath}});
    }

    const quint16 port = m_config.port > 0 ? static_cast<quint16>(m_config.port) : reservePort();
    if (port == 0) {
        return OperationResult::failure(QStringLiteral("infra"),
                                        QStringLiteral("Unable to reserve a free server port"));
    }

    const QString dbPath = m_config.dbPath.isEmpty()
                               ? m_tempDir->filePath(QStringLiteral("debug-cli.db"))
                               : m_config.dbPath;
    m_config.dbPath = dbPath;
    m_logPath       = m_tempDir->filePath(QStringLiteral("server.log"));

    m_effectiveConfig = {.scheme = m_config.scheme.toStdString(),
                         .address = m_config.address.toStdString(),
                         .port = static_cast<int>(port)};

    m_process.setProgram(binaryPath);
    m_process.setArguments({});
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_process.setStandardOutputFile(m_logPath);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("SAGESTORE_SERVER_SCHEME"), m_config.scheme);
    environment.insert(QStringLiteral("SAGESTORE_SERVER_ADDRESS"), m_config.address);
    environment.insert(QStringLiteral("SAGESTORE_SERVER_PORT"), QString::number(port));
    environment.insert(QStringLiteral("SAGESTORE_DB_PATH"), dbPath);
    environment.insert(QStringLiteral("SAGESTORE_DB_SQL_PATH"), sqlPath);
    m_process.setProcessEnvironment(environment);

    m_process.start();
    if (!m_process.waitForStarted(5000)) {
        return OperationResult::failure(
            QStringLiteral("infra"),
            QStringLiteral("Failed to start SageStoreServer: %1").arg(m_process.errorString()));
    }

    const OperationResult healthResult = health(m_config.startupTimeoutMs);
    if (!healthResult.ok) {
        stop();
        QJsonObject details = healthResult.result;
        details.insert(QStringLiteral("serverLog"), logContents());
        return OperationResult::failure(healthResult.category, healthResult.message, details);
    }

    m_prepared = true;
    return OperationResult::success(state());
}

OperationResult ServerController::request(const QString &method, const QString &endpoint,
                                          const Dataset &dataset, const QString &resourceId,
                                          int timeoutMs)
{
    if (!m_prepared && m_config.mode.compare(QStringLiteral("spawn"), Qt::CaseInsensitive) != 0) {
        return OperationResult::failure(
            QStringLiteral("infra"),
            QStringLiteral("Server controller is not prepared before request execution"));
    }

    QString normalizedMethod = method.trimmed().toUpper();
    if (normalizedMethod == QStringLiteral("DELETE")) {
        normalizedMethod = QStringLiteral("DEL");
    }

    QUrl url;
    url.setScheme(QString::fromStdString(m_effectiveConfig.scheme));
    url.setHost(QString::fromStdString(m_effectiveConfig.address));
    url.setPort(m_effectiveConfig.port);

    QString path = endpoint;
    if (!path.startsWith('/')) {
        path.prepend('/');
    }
    if (!resourceId.isEmpty()) {
        path += QStringLiteral("/") + resourceId;
    }
    url.setPath(path);

    QNetworkRequest networkRequest(url);
    networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                                QNetworkRequest::ManualRedirectPolicy);

    QNetworkAccessManager manager;
    QNetworkReply *reply = nullptr;
    const QByteArray payload = serializeDataset(dataset);

    if (normalizedMethod == QStringLiteral("GET")) {
        reply = manager.get(networkRequest);
    } else if (normalizedMethod == QStringLiteral("POST")) {
        reply = manager.post(networkRequest, payload);
    } else if (normalizedMethod == QStringLiteral("PUT")) {
        reply = manager.put(networkRequest, payload);
    } else if (normalizedMethod == QStringLiteral("DEL")) {
        reply = manager.deleteResource(networkRequest);
    } else {
        return OperationResult::failure(
            QStringLiteral("scenario"),
            QStringLiteral("Unsupported HTTP method for server.request: %1").arg(method));
    }

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(std::max(timeoutMs, 1));
    loop.exec();

    if (reply->isRunning()) {
        reply->abort();
        reply->deleteLater();
        return OperationResult::failure(
            QStringLiteral("timeout"),
            QStringLiteral("Timed out waiting for server response from %1").arg(url.toString()),
            QJsonObject{{QStringLiteral("url"), url.toString()},
                        {QStringLiteral("method"), normalizedMethod}});
    }

    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray rawBody = reply->readAll();
    const Dataset responseDataset = deserializeDataset(rawBody);
    const QString errorMessage = datasetErrorMessage(responseDataset);

    QJsonObject details{
        {QStringLiteral("url"), url.toString()},
        {QStringLiteral("method"), normalizedMethod},
        {QStringLiteral("httpStatus"), httpStatus},
        {QStringLiteral("rawBody"), QString::fromUtf8(rawBody)},
        {QStringLiteral("dataset"), JsonUtils::datasetToJsonObject(responseDataset)},
    };

    if (reply->error() != QNetworkReply::NoError) {
        details.insert(QStringLiteral("networkError"), reply->errorString());
        reply->deleteLater();
        return OperationResult::failure(
            QStringLiteral("infra"),
            QStringLiteral("Network error for %1: %2").arg(url.toString(), reply->errorString()),
            details);
    }

    reply->deleteLater();

    if (!errorMessage.isEmpty()) {
        return OperationResult::failure(QStringLiteral("server_error"), errorMessage, details);
    }

    return OperationResult::success(details);
}

OperationResult ServerController::health(int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();

    OperationResult lastFailure = OperationResult::failure(
        QStringLiteral("timeout"), QStringLiteral("Server health endpoint did not become ready"));

    while (timer.elapsed() < timeoutMs) {
        if (m_config.mode.compare(QStringLiteral("spawn"), Qt::CaseInsensitive) == 0 &&
            m_process.state() == QProcess::NotRunning) {
            return OperationResult::failure(
                QStringLiteral("infra"),
                QStringLiteral("SageStoreServer stopped before reporting ready"),
                QJsonObject{{QStringLiteral("serverLog"), logContents()}});
        }

        const int requestTimeoutMs = std::max(200, std::min(1000, timeoutMs - static_cast<int>(timer.elapsed())));
        const OperationResult requestResult =
            request(QStringLiteral("GET"), QString::fromLatin1(Endpoints::System::HEALTH), {}, {},
                    requestTimeoutMs);
        if (requestResult.ok) {
            const QJsonObject dataset =
                requestResult.result.value(QStringLiteral("dataset")).toObject();
            const QJsonArray statusValues = dataset.value(QStringLiteral("status")).toArray();
            const QJsonArray readyValues  = dataset.value(QStringLiteral("ready")).toArray();
            const QString status =
                statusValues.isEmpty() ? QString() : statusValues.first().toString();
            const QString ready =
                readyValues.isEmpty() ? QString() : readyValues.first().toString();

            if (status == QStringLiteral("ok") && ready == QStringLiteral("true")) {
                return requestResult;
            }

            QJsonObject details = requestResult.result;
            details.insert(QStringLiteral("expectedStatus"), QStringLiteral("ok"));
            details.insert(QStringLiteral("expectedReady"), QStringLiteral("true"));
            lastFailure = OperationResult::failure(
                QStringLiteral("server_error"),
                QStringLiteral("Server health endpoint did not report ready"), details);
        } else {
            lastFailure = requestResult;
        }

        QThread::msleep(100);
    }

    if (m_config.mode.compare(QStringLiteral("spawn"), Qt::CaseInsensitive) == 0) {
        QJsonObject details = lastFailure.result;
        details.insert(QStringLiteral("serverLog"), logContents());
        return OperationResult::failure(lastFailure.category, lastFailure.message, details);
    }

    return lastFailure;
}

void ServerController::stop()
{
    if (m_process.state() == QProcess::NotRunning) {
        return;
    }

    m_process.terminate();
    if (!m_process.waitForFinished(3000)) {
        m_process.kill();
        m_process.waitForFinished(3000);
    }
}

bool ServerController::isPrepared() const { return m_prepared; }

QJsonObject ServerController::state() const
{
    return QJsonObject{
        {QStringLiteral("mode"), m_config.mode},
        {QStringLiteral("scheme"), QString::fromStdString(m_effectiveConfig.scheme)},
        {QStringLiteral("address"), QString::fromStdString(m_effectiveConfig.address)},
        {QStringLiteral("port"), m_effectiveConfig.port},
        {QStringLiteral("started"), m_process.state() == QProcess::Running},
        {QStringLiteral("binaryPath"),
         m_config.binaryPath.isEmpty() ? QStringLiteral(SAGESTORE_DEBUGCLI_DEFAULT_SERVER_BIN)
                                       : m_config.binaryPath},
        {QStringLiteral("sqlPath"),
         m_config.sqlPath.isEmpty() ? QStringLiteral(SAGESTORE_DEBUGCLI_DEFAULT_SERVER_SQL)
                                    : m_config.sqlPath},
        {QStringLiteral("dbPath"), m_config.dbPath},
        {QStringLiteral("logPath"), m_logPath},
        {QStringLiteral("processId"),
         QString::number(static_cast<qulonglong>(m_process.processId()))},
    };
}

NetworkService::ServerConfig ServerController::serverConfig() const { return m_effectiveConfig; }

quint16 ServerController::reservePort()
{
    QTcpServer server;
    if (!server.listen(QHostAddress::LocalHost, 0)) {
        return 0;
    }

    return server.serverPort();
}

QString ServerController::logContents() const
{
    if (m_logPath.isEmpty()) {
        return {};
    }

    QFile file(m_logPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    return QString::fromUtf8(file.readAll());
}

ServerController::Config ServerController::normalizedConfig(const Config &config) const
{
    Config normalized = config;
    if (normalized.mode.isEmpty()) {
        normalized.mode = QStringLiteral("target");
    }
    if (normalized.scheme.isEmpty()) {
        normalized.scheme = QStringLiteral("http");
    }
    if (normalized.address.isEmpty()) {
        normalized.address = QStringLiteral("127.0.0.1");
    }
    if (normalized.port <= 0 &&
        normalized.mode.compare(QStringLiteral("spawn"), Qt::CaseInsensitive) != 0) {
        normalized.port = 8001;
    }
    if (normalized.startupTimeoutMs <= 0) {
        normalized.startupTimeoutMs = 10000;
    }
    return normalized;
}

} // namespace DebugCli
