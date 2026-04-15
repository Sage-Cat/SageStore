#include "DebugCliRunner.hpp"

#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QThread>

#include <algorithm>

#include "JsonUtils.hpp"

namespace DebugCli {
namespace {
QString stepAction(const QJsonObject &step)
{
    return step.value(QStringLiteral("action")).toString();
}
} // namespace

DebugCliRunner::DebugCliRunner(QApplication &application)
    : m_application(application), m_uiAutomation(application)
{
}

DebugCliRunner::RunResult DebugCliRunner::run(const QStringList &arguments)
{
    if (arguments.size() < 2) {
        return usageError(QStringLiteral("Missing command. Use 'describe' or 'run'."));
    }

    const QString command = arguments.at(1).trimmed();
    if (command == QStringLiteral("describe")) {
        return runDescribe();
    }

    if (command != QStringLiteral("run")) {
        return usageError(QStringLiteral("Unknown command '%1'").arg(command));
    }

    QString scenarioPath;
    QString resultPath;
    QString artifactDir;
    for (int index = 2; index < arguments.size(); ++index) {
        const QString argument = arguments.at(index);
        if (argument == QStringLiteral("--scenario") && index + 1 < arguments.size()) {
            scenarioPath = arguments.at(++index);
        } else if (argument == QStringLiteral("--result") && index + 1 < arguments.size()) {
            resultPath = arguments.at(++index);
        } else if (argument == QStringLiteral("--artifact-dir") && index + 1 < arguments.size()) {
            artifactDir = arguments.at(++index);
        } else {
            return usageError(QStringLiteral("Unknown or incomplete argument '%1'").arg(argument));
        }
    }

    if (scenarioPath.isEmpty()) {
        return usageError(QStringLiteral("The run command requires --scenario <path>."));
    }

    return runScenario(scenarioPath, resultPath, artifactDir);
}

DebugCliRunner::RunResult DebugCliRunner::usageError(const QString &message) const
{
    return RunResult{
        .exitCode = exitCodeForCategory(QStringLiteral("usage")),
        .document = QJsonObject{{QStringLiteral("ok"), false},
                                {QStringLiteral("category"), QStringLiteral("usage")},
                                {QStringLiteral("message"), message}}};
}

DebugCliRunner::RunResult DebugCliRunner::runDescribe() const
{
    QJsonArray actions;
    for (const QString &action :
         {QStringLiteral("server.health"), QStringLiteral("server.request"),
          QStringLiteral("client.start"), QStringLiteral("client.login"),
          QStringLiteral("ui.triggerMenu"), QStringLiteral("ui.triggerAction"),
          QStringLiteral("ui.click"), QStringLiteral("ui.setText"),
          QStringLiteral("ui.setComboText"), QStringLiteral("ui.selectTableRow"),
          QStringLiteral("ui.editTableCell"), QStringLiteral("ui.waitVisible"),
          QStringLiteral("ui.waitText"), QStringLiteral("ui.waitTableRows"),
          QStringLiteral("ui.capture"), QStringLiteral("ui.dumpState"),
          QStringLiteral("ui.sleep")}) {
        actions.append(action);
    }

    const QJsonObject document{
        {QStringLiteral("ok"), true},
        {QStringLiteral("command"), QStringLiteral("describe")},
        {QStringLiteral("qtPlatform"), m_application.platformName()},
        {QStringLiteral("supportedActions"), actions},
        {QStringLiteral("menuActions"), describeMenuActions()},
        {QStringLiteral("exitCodes"),
         QJsonObject{{QStringLiteral("success"), 0},
                     {QStringLiteral("usage"), exitCodeForCategory(QStringLiteral("usage"))},
                     {QStringLiteral("scenario"), exitCodeForCategory(QStringLiteral("scenario"))},
                     {QStringLiteral("infra"), exitCodeForCategory(QStringLiteral("infra"))},
                     {QStringLiteral("selector"), exitCodeForCategory(QStringLiteral("selector"))},
                     {QStringLiteral("timeout"), exitCodeForCategory(QStringLiteral("timeout"))},
                     {QStringLiteral("server_error"),
                      exitCodeForCategory(QStringLiteral("server_error"))},
                     {QStringLiteral("assertion"),
                      exitCodeForCategory(QStringLiteral("assertion"))}}}};

    return RunResult{.exitCode = 0, .document = document};
}

DebugCliRunner::RunResult DebugCliRunner::runScenario(const QString &scenarioPath,
                                                      const QString &resultPath,
                                                      const QString &artifactDir)
{
    QJsonDocument scenarioDocument;
    QString errorMessage;
    if (!JsonUtils::readJsonFile(scenarioPath, &scenarioDocument, &errorMessage)) {
        return RunResult{.exitCode = exitCodeForCategory(QStringLiteral("scenario")),
                         .document = QJsonObject{{QStringLiteral("ok"), false},
                                                 {QStringLiteral("category"),
                                                  QStringLiteral("scenario")},
                                                 {QStringLiteral("message"), errorMessage}}};
    }

    const QJsonObject scenario = scenarioDocument.object();
    const QDir scenarioDir(QFileInfo(scenarioPath).absoluteDir());
    const QString resolvedArtifactDir =
        artifactDir.isEmpty() ? QString() : QFileInfo(artifactDir).absoluteFilePath();

    const ServerController::Config serverConfig =
        ServerController::configFromJson(scenario.value(QStringLiteral("server")).toObject());
    UiAutomation::Options clientOptions;
    clientOptions.autoAcceptDialogs =
        scenario.value(QStringLiteral("client")).toObject().value(QStringLiteral("autoAcceptDialogs"))
            .toBool(true);

    QJsonArray stepResults;
    bool overallOk        = true;
    QString failureCategory;
    QString failureMessage;
    int failureStepIndex = -1;

    const QJsonArray steps = scenario.value(QStringLiteral("steps")).toArray();
    for (int index = 0; index < steps.size(); ++index) {
        const QJsonObject step = steps.at(index).toObject();
        const QString action = stepAction(step);
        const int timeoutMs = step.value(QStringLiteral("timeoutMs")).toInt(5000);

        QElapsedTimer timer;
        timer.start();

        OperationResult operationResult = OperationResult::failure(
            QStringLiteral("scenario"),
            QStringLiteral("Unhandled or missing action at step %1").arg(index));

        if (action == QStringLiteral("server.health")) {
            const OperationResult prepareResult = m_serverController.prepare(serverConfig);
            operationResult = prepareResult.ok ? m_serverController.health(timeoutMs) : prepareResult;
        } else if (action == QStringLiteral("server.request")) {
            const OperationResult prepareResult = m_serverController.prepare(serverConfig);
            if (prepareResult.ok) {
                operationResult = m_serverController.request(
                    step.value(QStringLiteral("method")).toString(QStringLiteral("GET")),
                    step.value(QStringLiteral("endpoint")).toString(),
                    JsonUtils::datasetFromJsonObject(
                        step.value(QStringLiteral("dataset")).toObject()),
                    step.value(QStringLiteral("resourceId")).toString(), timeoutMs);
            } else {
                operationResult = prepareResult;
            }
        } else if (action == QStringLiteral("client.start")) {
            const OperationResult prepareResult = m_serverController.prepare(serverConfig);
            if (prepareResult.ok) {
                operationResult =
                    m_uiAutomation.start(m_serverController.serverConfig(), clientOptions);
            } else {
                operationResult = prepareResult;
            }
        } else if (action == QStringLiteral("client.login")) {
            operationResult = m_uiAutomation.login(
                step.value(QStringLiteral("username")).toString(QStringLiteral("admin")),
                step.value(QStringLiteral("password")).toString(QStringLiteral("admin123")),
                timeoutMs);
        } else if (action == QStringLiteral("ui.triggerMenu")) {
            operationResult =
                m_uiAutomation.triggerMenu(step.value(QStringLiteral("menu")).toString());
        } else if (action == QStringLiteral("ui.triggerAction")) {
            operationResult =
                m_uiAutomation.triggerAction(step.value(QStringLiteral("objectName")).toString());
        } else if (action == QStringLiteral("ui.click")) {
            operationResult =
                m_uiAutomation.click(step.value(QStringLiteral("objectName")).toString());
        } else if (action == QStringLiteral("ui.setText")) {
            operationResult =
                m_uiAutomation.setText(step.value(QStringLiteral("objectName")).toString(),
                                       step.value(QStringLiteral("text")).toString());
        } else if (action == QStringLiteral("ui.setComboText")) {
            operationResult =
                m_uiAutomation.setComboText(step.value(QStringLiteral("objectName")).toString(),
                                            step.value(QStringLiteral("text")).toString());
        } else if (action == QStringLiteral("ui.selectTableRow")) {
            operationResult = m_uiAutomation.selectTableRow(
                step.value(QStringLiteral("objectName")).toString(),
                step.value(QStringLiteral("row")).toInt(-1));
        } else if (action == QStringLiteral("ui.editTableCell")) {
            const QString cellText =
                step.value(QStringLiteral("text"))
                    .toString(step.value(QStringLiteral("comboText")).toString());
            operationResult = m_uiAutomation.editTableCell(
                step.value(QStringLiteral("objectName")).toString(),
                step.value(QStringLiteral("row")).toInt(-1),
                step.value(QStringLiteral("column")).toInt(-1), cellText,
                step.contains(QStringLiteral("comboText")));
        } else if (action == QStringLiteral("ui.waitVisible")) {
            operationResult = m_uiAutomation.waitVisible(
                step.value(QStringLiteral("objectName")).toString(), timeoutMs);
        } else if (action == QStringLiteral("ui.waitText")) {
            operationResult = m_uiAutomation.waitText(
                step.value(QStringLiteral("objectName")).toString(),
                step.value(QStringLiteral("text")).toString(), timeoutMs);
        } else if (action == QStringLiteral("ui.waitTableRows")) {
            operationResult = m_uiAutomation.waitTableRows(
                step.value(QStringLiteral("objectName")).toString(),
                step.value(QStringLiteral("minRows")).toInt(1), timeoutMs);
        } else if (action == QStringLiteral("ui.capture")) {
            QString screenshotPath =
                resolveOutputPath(step.value(QStringLiteral("screenshotPath")).toString(),
                                  scenarioDir, resolvedArtifactDir);
            QString statePath =
                resolveOutputPath(step.value(QStringLiteral("statePath")).toString(), scenarioDir,
                                  resolvedArtifactDir);
            if (screenshotPath.isEmpty() && statePath.isEmpty() && !resolvedArtifactDir.isEmpty()) {
                screenshotPath = QDir(resolvedArtifactDir)
                                     .filePath(QStringLiteral("step-%1.png").arg(index));
                statePath = QDir(resolvedArtifactDir)
                                .filePath(QStringLiteral("step-%1.json").arg(index));
            }
            operationResult = m_uiAutomation.capture(
                step.value(QStringLiteral("objectName")).toString(), screenshotPath, statePath);
        } else if (action == QStringLiteral("ui.dumpState")) {
            QString statePath =
                resolveOutputPath(step.value(QStringLiteral("statePath")).toString(), scenarioDir,
                                  resolvedArtifactDir);
            if (statePath.isEmpty() && !resolvedArtifactDir.isEmpty()) {
                statePath = QDir(resolvedArtifactDir)
                                .filePath(QStringLiteral("step-%1.json").arg(index));
            }
            operationResult =
                m_uiAutomation.dumpState(step.value(QStringLiteral("objectName")).toString(), statePath);
        } else if (action == QStringLiteral("ui.sleep")) {
            const int sleepMs = std::max(step.value(QStringLiteral("durationMs")).toInt(timeoutMs), 0);
            QThread::msleep(static_cast<unsigned long>(sleepMs));
            operationResult =
                OperationResult::success(QJsonObject{{QStringLiteral("durationMs"), sleepMs}});
        }

        QJsonObject stepResult{
            {QStringLiteral("index"), index},
            {QStringLiteral("action"), action},
            {QStringLiteral("ok"), operationResult.ok},
            {QStringLiteral("category"), operationResult.category},
            {QStringLiteral("message"), operationResult.message},
            {QStringLiteral("elapsedMs"), static_cast<qint64>(timer.elapsed())},
            {QStringLiteral("result"), operationResult.result},
        };

        if (!operationResult.ok && !resolvedArtifactDir.isEmpty() && m_uiAutomation.isStarted()) {
            const OperationResult failureArtifacts =
                m_uiAutomation.captureFailureArtifacts(resolvedArtifactDir, index);
            if (failureArtifacts.ok) {
                stepResult.insert(QStringLiteral("failureArtifacts"), failureArtifacts.result);
            }
        }

        stepResults.append(stepResult);

        if (!operationResult.ok) {
            overallOk = false;
            failureCategory = operationResult.category;
            failureMessage = operationResult.message;
            failureStepIndex = index;
            break;
        }
    }

    QJsonObject document{
        {QStringLiteral("ok"), overallOk},
        {QStringLiteral("command"), QStringLiteral("run")},
        {QStringLiteral("scenarioPath"), QFileInfo(scenarioPath).absoluteFilePath()},
        {QStringLiteral("artifactDir"), resolvedArtifactDir},
        {QStringLiteral("qtPlatform"), m_application.platformName()},
        {QStringLiteral("server"), m_serverController.state()},
        {QStringLiteral("ui"), m_uiAutomation.info()},
        {QStringLiteral("steps"), stepResults},
    };

    if (!overallOk) {
        document.insert(QStringLiteral("category"), failureCategory);
        document.insert(QStringLiteral("message"), failureMessage);
        document.insert(QStringLiteral("failedStepIndex"), failureStepIndex);
    }

    if (!resultPath.isEmpty()) {
        const QString resolvedResultPath =
            resolveOutputPath(resultPath, scenarioDir, resolvedArtifactDir);
        QString writeError;
        if (!JsonUtils::writeJsonFile(resolvedResultPath, QJsonDocument(document), &writeError)) {
            return RunResult{.exitCode = exitCodeForCategory(QStringLiteral("infra")),
                             .document = QJsonObject{{QStringLiteral("ok"), false},
                                                     {QStringLiteral("category"),
                                                      QStringLiteral("infra")},
                                                     {QStringLiteral("message"), writeError},
                                                     {QStringLiteral("partialResult"), document}}};
        }
        document.insert(QStringLiteral("resultPath"), resolvedResultPath);
    }

    return RunResult{.exitCode = overallOk ? 0 : exitCodeForCategory(failureCategory),
                     .document = document};
}

QString DebugCliRunner::resolveOutputPath(const QString &rawPath, const QDir &scenarioDir,
                                          const QString &artifactDir) const
{
    if (rawPath.isEmpty()) {
        return {};
    }

    const QFileInfo rawInfo(rawPath);
    if (rawInfo.isAbsolute()) {
        return rawInfo.absoluteFilePath();
    }

    if (!artifactDir.isEmpty()) {
        return QDir(artifactDir).filePath(rawPath);
    }

    return scenarioDir.absoluteFilePath(rawPath);
}

int DebugCliRunner::exitCodeForCategory(const QString &category) const
{
    if (category == QStringLiteral("usage")) {
        return 10;
    }
    if (category == QStringLiteral("scenario")) {
        return 11;
    }
    if (category == QStringLiteral("infra")) {
        return 20;
    }
    if (category == QStringLiteral("selector")) {
        return 30;
    }
    if (category == QStringLiteral("timeout")) {
        return 40;
    }
    if (category == QStringLiteral("server_error")) {
        return 50;
    }
    if (category == QStringLiteral("assertion")) {
        return 60;
    }

    return 1;
}

} // namespace DebugCli
