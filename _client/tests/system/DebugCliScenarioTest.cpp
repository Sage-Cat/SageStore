#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTemporaryDir>
#include <QtTest>

#include "wrappers/LogAudit.hpp"
#include "wrappers/UiArtifactAudit.hpp"

namespace {
struct ScenarioExecution {
    QProcess::ExitStatus exitStatus{QProcess::CrashExit};
    int exitCode{-1};
    QByteArray standardOutput;
    QByteArray standardError;
    QJsonObject result;
};

struct WorkspaceCapture {
    QString menu;
    QString tableObjectName;
    int minRows{1};
    QString screenshotName;
    QString stateName;
    QStringList expectedObjectNames;
};

void writeJsonFile(const QString &path, const QJsonObject &object)
{
    QFile file(path);
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text));
    file.write(QJsonDocument(object).toJson(QJsonDocument::Indented));
}

QJsonDocument loadJsonFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }

    return QJsonDocument::fromJson(file.readAll());
}

ScenarioExecution executeScenario(const QString &scenarioPath, const QString &resultPath,
                                  const QString &artifactDir, const QString &logDir)
{
    ScenarioExecution execution;
    if (!QFile::exists(QStringLiteral(SAGESTORE_DEBUGCLI_BIN))) {
        return execution;
    }

    QProcess process;
    process.setProgram(QStringLiteral(SAGESTORE_DEBUGCLI_BIN));
    process.setArguments({QStringLiteral("run"), QStringLiteral("--scenario"), scenarioPath,
                          QStringLiteral("--result"), resultPath,
                          QStringLiteral("--artifact-dir"), artifactDir});

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("offscreen"));
    environment.insert(QStringLiteral("SAGESTORE_FORCE_OFFSCREEN"), QStringLiteral("1"));
    environment.insert(QStringLiteral("SAGESTORE_LOG_DIR"), logDir);
    process.setProcessEnvironment(environment);

    process.start();
    if (!process.waitForStarted()) {
        return execution;
    }

    if (!process.waitForFinished(120000)) {
        process.kill();
        process.waitForFinished(5000);
    }

    execution.exitStatus = process.exitStatus();
    execution.exitCode = process.exitCode();
    execution.standardOutput = process.readAllStandardOutput();
    execution.standardError = process.readAllStandardError();

    if (QFile::exists(resultPath)) {
        execution.result = loadJsonFile(resultPath).object();
    }

    return execution;
}

void assertCaptureStepHealthy(const QJsonObject &captureStep, const QStringList &expectedObjectNames)
{
    QCOMPARE(captureStep.value(QStringLiteral("ok")).toBool(), true);

    const QJsonObject captureResult = captureStep.value(QStringLiteral("result")).toObject();
    const QString screenshotPath =
        captureResult.value(QStringLiteral("screenshotPath")).toString();
    const QString statePath = captureResult.value(QStringLiteral("statePath")).toString();
    QVERIFY2(QFile::exists(screenshotPath), qPrintable(screenshotPath));
    QVERIFY2(QFile::exists(statePath), qPrintable(statePath));

    const QString screenshotAuditError = UiArtifactAudit::auditScreenshot(screenshotPath);
    QVERIFY2(screenshotAuditError.isEmpty(), qPrintable(screenshotAuditError));

    const QJsonDocument stateDocument = loadJsonFile(statePath);
    const QString stateAuditError =
        UiArtifactAudit::auditStateDocument(stateDocument, expectedObjectNames);
    QVERIFY2(stateAuditError.isEmpty(), qPrintable(stateAuditError));
}

void assertLogsClean(const QJsonObject &result, const QString &logDir)
{
    QStringList logTargets;
    logTargets.append(logDir);

    const QStringList producedLogs =
        QDir(logDir).entryList(QStringList{QStringLiteral("*.log")}, QDir::Files);
    QVERIFY2(!producedLogs.isEmpty(), qPrintable(logDir));

    const QString serverLogPath =
        result.value(QStringLiteral("server")).toObject().value(QStringLiteral("logPath")).toString();
    if (!serverLogPath.isEmpty() && QFile::exists(serverLogPath)) {
        logTargets.append(serverLogPath);
    }

    const QVector<LogAudit::Finding> findings = LogAudit::collectFindings(logTargets);
    QVERIFY2(findings.isEmpty(), qPrintable(LogAudit::formatFindings(findings)));
}
} // namespace

class DebugCliScenarioTest : public QObject {
    Q_OBJECT

private slots:
    void testDescribeOutputsSupportedSurface()
    {
        QVERIFY2(QFile::exists(QStringLiteral(SAGESTORE_DEBUGCLI_BIN)),
                 "SageStoreDebugCli binary was not built");

        QProcess process;
        process.setProgram(QStringLiteral(SAGESTORE_DEBUGCLI_BIN));
        process.setArguments({QStringLiteral("describe")});
        QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
        environment.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("offscreen"));
        process.setProcessEnvironment(environment);

        process.start();
        QVERIFY(process.waitForStarted());
        QVERIFY2(process.waitForFinished(30000), qPrintable(process.errorString()));
        QCOMPARE(process.exitStatus(), QProcess::NormalExit);
        QCOMPARE(process.exitCode(), 0);

        const QJsonDocument output = QJsonDocument::fromJson(process.readAllStandardOutput());
        QVERIFY(output.isObject());
        const QJsonObject object = output.object();
        QCOMPARE(object.value(QStringLiteral("ok")).toBool(), true);
        const QJsonArray actions = object.value(QStringLiteral("supportedActions")).toArray();
        QVERIFY(actions.contains(QStringLiteral("ui.capture")));
        QVERIFY(actions.contains(QStringLiteral("server.request")));
    }

    void testScenarioRunsAndAuditsArtifactsAndLogs()
    {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        const QString artifactDir = tempDir.filePath(QStringLiteral("artifacts"));
        const QString logDir = tempDir.filePath(QStringLiteral("logs"));
        const QString scenarioPath = tempDir.filePath(QStringLiteral("scenario.json"));
        const QString resultPath = tempDir.filePath(QStringLiteral("result.json"));
        QVERIFY(QDir().mkpath(artifactDir));
        QVERIFY(QDir().mkpath(logDir));

        const QJsonObject scenario{
            {QStringLiteral("server"),
             QJsonObject{{QStringLiteral("mode"), QStringLiteral("spawn")}}},
            {QStringLiteral("client"),
             QJsonObject{{QStringLiteral("autoAcceptDialogs"), true}}},
            {QStringLiteral("steps"),
             QJsonArray{
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("server.health")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("client.start")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("client.login")},
                             {QStringLiteral("username"), QStringLiteral("admin")},
                             {QStringLiteral("password"), QStringLiteral("admin123")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.triggerMenu")},
                             {QStringLiteral("menu"), QStringLiteral("PRODUCT_MANAGEMENT")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.waitTableRows")},
                             {QStringLiteral("objectName"), QStringLiteral("productTypesTable")},
                             {QStringLiteral("minRows"), 1}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.capture")},
                             {QStringLiteral("objectName"), QStringLiteral("mainWindow")},
                             {QStringLiteral("screenshotPath"),
                              QStringLiteral("product-management.png")},
                             {QStringLiteral("statePath"),
                              QStringLiteral("product-management.json")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("server.request")},
                             {QStringLiteral("method"), QStringLiteral("GET")},
                             {QStringLiteral("endpoint"),
                              QStringLiteral("/api/system/health")}},
             }},
        };

        writeJsonFile(scenarioPath, scenario);
        const ScenarioExecution execution =
            executeScenario(scenarioPath, resultPath, artifactDir, logDir);

        QCOMPARE(execution.exitStatus, QProcess::NormalExit);
        QCOMPARE(execution.exitCode, 0);
        QVERIFY2(QFile::exists(resultPath),
                 qPrintable(QString::fromUtf8(execution.standardOutput)));

        const QJsonObject result = execution.result;
        QCOMPARE(result.value(QStringLiteral("ok")).toBool(), true);
        const QJsonArray steps = result.value(QStringLiteral("steps")).toArray();
        QCOMPARE(steps.size(), 7);

        assertCaptureStepHealthy(steps.at(5).toObject(),
                                 {QStringLiteral("mainWindow"),
                                  QStringLiteral("productTypesTable"),
                                  QStringLiteral("productTypesAddButton")});
        assertLogsClean(result, logDir);
    }

    void testImportantWorkspacesRenderOffscreenAndStayClean()
    {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        const QString artifactDir = tempDir.filePath(QStringLiteral("artifacts"));
        const QString logDir = tempDir.filePath(QStringLiteral("logs"));
        const QString scenarioPath = tempDir.filePath(QStringLiteral("workspace-scenario.json"));
        const QString resultPath = tempDir.filePath(QStringLiteral("workspace-result.json"));
        QVERIFY(QDir().mkpath(artifactDir));
        QVERIFY(QDir().mkpath(logDir));

        const QList<WorkspaceCapture> workspaces = {
            {QStringLiteral("PRODUCT_MANAGEMENT"), QStringLiteral("productTypesTable"), 1,
             QStringLiteral("product-management.png"), QStringLiteral("product-management.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("productTypesTable"),
              QStringLiteral("productTypesAddButton")}},
            {QStringLiteral("STOCK_TRACKING"), QStringLiteral("stocksTable"), 1,
             QStringLiteral("stock-tracking.png"), QStringLiteral("stock-tracking.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("stocksTable")}},
            {QStringLiteral("CUSTOMERS"), QStringLiteral("managementContactsTable"), 1,
             QStringLiteral("management.png"), QStringLiteral("management.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("managementContactsTable"),
              QStringLiteral("managementSuppliersTable"),
              QStringLiteral("managementEmployeesTable")}},
            {QStringLiteral("PURCHASE_ORDERS"), QStringLiteral("purchaseOrdersTable"), 1,
             QStringLiteral("purchase-orders.png"), QStringLiteral("purchase-orders.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("purchaseOrdersTable"),
              QStringLiteral("purchaseReceiptOrdersTable")}},
            {QStringLiteral("SALES_ORDERS"), QStringLiteral("salesOrdersTable"), 1,
             QStringLiteral("sales-orders.png"), QStringLiteral("sales-orders.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("salesOrdersTable")}},
            {QStringLiteral("INVOICING"), QStringLiteral("salesInvoiceOrdersTable"), 1,
             QStringLiteral("invoicing.png"), QStringLiteral("invoicing.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("salesInvoiceOrdersTable"),
              QStringLiteral("salesInvoicePreview")}},
            {QStringLiteral("USERS"), QStringLiteral("usersTable"), 1,
             QStringLiteral("users.png"), QStringLiteral("users.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("usersTable")}},
            {QStringLiteral("USER_ROLES"), QStringLiteral("rolesTable"), 2,
             QStringLiteral("roles.png"), QStringLiteral("roles.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("rolesTable")}},
            {QStringLiteral("SALES_ANALYTICS"), QStringLiteral("analyticsSalesTable"), 1,
             QStringLiteral("analytics.png"), QStringLiteral("analytics.json"),
             {QStringLiteral("mainWindow"), QStringLiteral("analyticsSalesTable"),
              QStringLiteral("analyticsInventoryTable")}},
        };

        QJsonArray steps{
            QJsonObject{{QStringLiteral("action"), QStringLiteral("server.health")}},
            QJsonObject{{QStringLiteral("action"), QStringLiteral("client.start")}},
            QJsonObject{{QStringLiteral("action"), QStringLiteral("client.login")},
                        {QStringLiteral("username"), QStringLiteral("admin")},
                        {QStringLiteral("password"), QStringLiteral("admin123")}},
        };

        for (const WorkspaceCapture &workspace : workspaces) {
            steps.append(QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.triggerMenu")},
                                     {QStringLiteral("menu"), workspace.menu}});
            steps.append(QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.waitTableRows")},
                                     {QStringLiteral("objectName"), workspace.tableObjectName},
                                     {QStringLiteral("minRows"), workspace.minRows}});
            steps.append(QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.capture")},
                                     {QStringLiteral("objectName"), QStringLiteral("mainWindow")},
                                     {QStringLiteral("screenshotPath"), workspace.screenshotName},
                                     {QStringLiteral("statePath"), workspace.stateName}});
        }

        writeJsonFile(scenarioPath, QJsonObject{
                                       {QStringLiteral("server"),
                                        QJsonObject{{QStringLiteral("mode"),
                                                     QStringLiteral("spawn")}}},
                                       {QStringLiteral("client"),
                                        QJsonObject{{QStringLiteral("autoAcceptDialogs"), true}}},
                                       {QStringLiteral("steps"), steps},
                                   });

        const ScenarioExecution execution =
            executeScenario(scenarioPath, resultPath, artifactDir, logDir);

        QCOMPARE(execution.exitStatus, QProcess::NormalExit);
        QCOMPARE(execution.exitCode, 0);
        QVERIFY2(QFile::exists(resultPath),
                 qPrintable(QString::fromUtf8(execution.standardOutput)));

        const QJsonObject result = execution.result;
        QCOMPARE(result.value(QStringLiteral("ok")).toBool(), true);

        const QJsonArray stepResults = result.value(QStringLiteral("steps")).toArray();
        QCOMPARE(stepResults.size(), steps.size());

        int captureStepIndex = 5;
        for (const WorkspaceCapture &workspace : workspaces) {
            assertCaptureStepHealthy(stepResults.at(captureStepIndex).toObject(),
                                     workspace.expectedObjectNames);
            captureStepIndex += 3;
        }

        assertLogsClean(result, logDir);
    }

    void testScenarioReportsSelectorFailureAndFailureArtifacts()
    {
        QTemporaryDir tempDir;
        QVERIFY(tempDir.isValid());

        const QString artifactDir = tempDir.filePath(QStringLiteral("artifacts"));
        const QString logDir = tempDir.filePath(QStringLiteral("logs"));
        const QString scenarioPath = tempDir.filePath(QStringLiteral("failure-scenario.json"));
        const QString resultPath = tempDir.filePath(QStringLiteral("failure-result.json"));
        QVERIFY(QDir().mkpath(artifactDir));
        QVERIFY(QDir().mkpath(logDir));

        const QJsonObject scenario{
            {QStringLiteral("server"),
             QJsonObject{{QStringLiteral("mode"), QStringLiteral("spawn")}}},
            {QStringLiteral("client"),
             QJsonObject{{QStringLiteral("autoAcceptDialogs"), true}}},
            {QStringLiteral("steps"),
             QJsonArray{
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("client.start")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("client.login")},
                             {QStringLiteral("username"), QStringLiteral("admin")},
                             {QStringLiteral("password"), QStringLiteral("admin123")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.triggerMenu")},
                             {QStringLiteral("menu"), QStringLiteral("PRODUCT_MANAGEMENT")}},
                 QJsonObject{{QStringLiteral("action"), QStringLiteral("ui.click")},
                             {QStringLiteral("objectName"), QStringLiteral("missingButton")}},
             }},
        };

        writeJsonFile(scenarioPath, scenario);
        const ScenarioExecution execution =
            executeScenario(scenarioPath, resultPath, artifactDir, logDir);

        QCOMPARE(execution.exitStatus, QProcess::NormalExit);
        QCOMPARE(execution.exitCode, 30);
        QVERIFY(QFile::exists(resultPath));

        const QJsonObject result = execution.result;
        QCOMPARE(result.value(QStringLiteral("ok")).toBool(), false);
        QCOMPARE(result.value(QStringLiteral("category")).toString(), QStringLiteral("selector"));
        QCOMPARE(result.value(QStringLiteral("failedStepIndex")).toInt(), 3);

        const QJsonArray steps = result.value(QStringLiteral("steps")).toArray();
        QCOMPARE(steps.size(), 4);
        const QJsonObject failureStep = steps.at(3).toObject();
        QCOMPARE(failureStep.value(QStringLiteral("category")).toString(),
                 QStringLiteral("selector"));

        const QJsonObject artifacts =
            failureStep.value(QStringLiteral("failureArtifacts")).toObject();
        const QString screenshotPath =
            artifacts.value(QStringLiteral("screenshotPath")).toString();
        const QString statePath = artifacts.value(QStringLiteral("statePath")).toString();
        QVERIFY(QFile::exists(screenshotPath));
        QVERIFY(QFile::exists(statePath));

        const QString screenshotAuditError = UiArtifactAudit::auditScreenshot(screenshotPath);
        QVERIFY2(screenshotAuditError.isEmpty(), qPrintable(screenshotAuditError));
        const QString stateAuditError = UiArtifactAudit::auditStateDocument(
            loadJsonFile(statePath),
            {QStringLiteral("mainWindow"), QStringLiteral("productTypesTable")});
        QVERIFY2(stateAuditError.isEmpty(), qPrintable(stateAuditError));
    }
};

QTEST_MAIN(DebugCliScenarioTest)
#include "DebugCliScenarioTest.moc"
