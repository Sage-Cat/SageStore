#pragma once

#include <QApplication>
#include <QJsonArray>
#include <QJsonObject>

#include <memory>

#include "Network/NetworkService.hpp"
#include "OperationResult.hpp"

class ApiManager;
class DialogManager;
class MainWindow;
class NetworkService;
class StartupController;

namespace DebugCli {

QJsonArray describeMenuActions();

class UiAutomation {
public:
    struct Options {
        bool autoAcceptDialogs{true};
    };

    explicit UiAutomation(QApplication &application);
    ~UiAutomation();

    OperationResult start(const NetworkService::ServerConfig &serverConfig, const Options &options);
    OperationResult login(const QString &username, const QString &password, int timeoutMs);
    OperationResult triggerMenu(const QString &menuName);
    OperationResult triggerAction(const QString &objectName);
    OperationResult click(const QString &objectName);
    OperationResult setText(const QString &objectName, const QString &text);
    OperationResult setComboText(const QString &objectName, const QString &text);
    OperationResult selectTableRow(const QString &objectName, int row);
    OperationResult editTableCell(const QString &objectName, int row, int column,
                                  const QString &text, bool comboMode);
    OperationResult waitVisible(const QString &objectName, int timeoutMs);
    OperationResult waitText(const QString &objectName, const QString &expectedText, int timeoutMs);
    OperationResult waitTableRows(const QString &objectName, int minRows, int timeoutMs);
    OperationResult dumpState(const QString &objectName, const QString &outputPath = {}) const;
    OperationResult capture(const QString &objectName, const QString &screenshotPath,
                            const QString &statePath) const;
    OperationResult captureFailureArtifacts(const QString &artifactDir, int stepIndex) const;

    bool isStarted() const;
    QJsonObject info() const;

private:
    QApplication &m_application;
    Options m_options;
    std::unique_ptr<NetworkService> m_networkService;
    std::unique_ptr<ApiManager> m_apiManager;
    std::unique_ptr<DialogManager> m_dialogManager;
    std::unique_ptr<StartupController> m_startupController;
    std::unique_ptr<MainWindow> m_mainWindow;
};

} // namespace DebugCli
