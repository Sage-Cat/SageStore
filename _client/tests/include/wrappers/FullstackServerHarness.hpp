#pragma once

#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTcpServer>
#include <QTemporaryDir>
#include <QTest>
#include <QTimer>
#include <QUrl>

#include "Network/NetworkService.hpp"
#include "common/Endpoints.hpp"

class FullstackServerHarness {
public:
    FullstackServerHarness() = default;
    ~FullstackServerHarness() { stop(); }

    bool start(QString *errorMessage = nullptr)
    {
        if (m_tempDir.isValid() == false) {
            return setError(errorMessage, QStringLiteral("Unable to create temporary test directory"));
        }

        const QString serverBinary = QStringLiteral(SAGESTORE_TEST_SERVER_BIN);
        const QString sqlPath = QStringLiteral(SAGESTORE_TEST_SERVER_SQL);
        if (QFile::exists(serverBinary) == false) {
            return setError(errorMessage,
                            QStringLiteral("Server binary not found: %1").arg(serverBinary));
        }
        if (QFile::exists(sqlPath) == false) {
            return setError(errorMessage,
                            QStringLiteral("Server SQL bootstrap not found: %1").arg(sqlPath));
        }

        m_port = reservePort();
        if (m_port == 0) {
            return setError(errorMessage, QStringLiteral("Unable to reserve a free TCP port"));
        }

        m_logPath = m_tempDir.filePath(QStringLiteral("server.log"));

        m_process.setProgram(serverBinary);
        m_process.setArguments({});
        m_process.setProcessChannelMode(QProcess::MergedChannels);
        m_process.setStandardOutputFile(m_logPath);

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert(QStringLiteral("SAGESTORE_SERVER_ADDRESS"), QStringLiteral("127.0.0.1"));
        env.insert(QStringLiteral("SAGESTORE_SERVER_PORT"), QString::number(m_port));
        env.insert(QStringLiteral("SAGESTORE_DB_PATH"),
                   m_tempDir.filePath(QStringLiteral("component-test.db")));
        env.insert(QStringLiteral("SAGESTORE_DB_SQL_PATH"), sqlPath);
        m_process.setProcessEnvironment(env);

        m_process.start();
        if (m_process.waitForStarted(5000) == false) {
            return setError(errorMessage,
                            QStringLiteral("Failed to start server process: %1")
                                .arg(m_process.errorString()));
        }

        if (waitForReady(10000) == false) {
            return setError(errorMessage,
                            QStringLiteral("Server did not become ready in time.\n%1")
                                .arg(logContents()));
        }

        return true;
    }

    void stop()
    {
        if (m_process.state() == QProcess::NotRunning) {
            return;
        }

        m_process.terminate();
        if (m_process.waitForFinished(3000) == false) {
            m_process.kill();
            m_process.waitForFinished(3000);
        }
    }

    NetworkService::ServerConfig serverConfig() const
    {
        return NetworkService::ServerConfig{
            .scheme = "http", .address = "127.0.0.1", .port = static_cast<int>(m_port)};
    }

    QString logContents() const
    {
        QFile logFile(m_logPath);
        if (logFile.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
            return {};
        }

        return QString::fromUtf8(logFile.readAll());
    }

private:
    static quint16 reservePort()
    {
        QTcpServer server;
        if (server.listen(QHostAddress::LocalHost, 0) == false) {
            return 0;
        }

        return server.serverPort();
    }

    bool waitForReady(int timeoutMs)
    {
        QNetworkAccessManager manager;
        QElapsedTimer timer;
        timer.start();

        while (timer.elapsed() < timeoutMs) {
            if (m_process.state() != QProcess::Running) {
                return false;
            }

            QNetworkRequest request(QUrl(QStringLiteral("http://127.0.0.1:%1%2")
                                             .arg(QString::number(m_port),
                                                  QString::fromLatin1(Endpoints::System::HEALTH))));
            QNetworkReply *reply = manager.get(request);
            QEventLoop loop;
            QTimer requestTimeout;
            requestTimeout.setSingleShot(true);
            QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            QObject::connect(&requestTimeout, &QTimer::timeout, &loop, &QEventLoop::quit);
            requestTimeout.start(500);
            loop.exec();

            const int statusCode =
                reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            const bool ready = reply->isFinished() && reply->error() == QNetworkReply::NoError &&
                               statusCode >= 200 && statusCode < 400;
            if (reply->isRunning()) {
                reply->abort();
            }
            reply->deleteLater();

            if (ready) {
                return true;
            }

            QTest::qWait(100);
        }

        return false;
    }

    static bool setError(QString *errorMessage, const QString &message)
    {
        if (errorMessage != nullptr) {
            *errorMessage = message;
        }
        return false;
    }

    mutable QProcess m_process;
    QTemporaryDir m_tempDir;
    QString m_logPath;
    quint16 m_port{0};
};
