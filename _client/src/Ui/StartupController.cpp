#include "StartupController.hpp"

#include "MainWindow.hpp"
#include "Ui/Dialogs/DialogManager.hpp"

StartupController::StartupController(DialogManager &dialogManager,
                                     MainWindowFactory mainWindowFactory, QObject *parent)
    : QObject(parent), m_dialogManager(dialogManager),
      m_mainWindowFactory(std::move(mainWindowFactory))
{
    connect(&m_dialogManager, &DialogManager::loginSucceeded, this,
            &StartupController::onLoginSucceeded);
}

void StartupController::start() { m_dialogManager.showLoginDialog(); }

MainWindow *StartupController::mainWindow() const { return m_mainWindow; }

void StartupController::onLoginSucceeded()
{
    if (m_mainWindow == nullptr) {
        m_mainWindow = m_mainWindowFactory();
        if (m_mainWindow == nullptr) {
            return;
        }
    }

    m_mainWindow->show();
    m_mainWindow->activateWindow();
}
