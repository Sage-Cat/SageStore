#pragma once

#include <QObject>
#include <QApplication>
#include <QFont>
#include <QString>
#include <QMap>

// Networking
class ApiManager;

// Ui
class MainWindow;
class DialogManager;

/**
 * @class UiManager
 * @brief Manages UI components.
 */
class UiManager : public QObject
{
    Q_OBJECT

public:
    explicit UiManager(QApplication &app, ApiManager &apiClient) noexcept;
    virtual ~UiManager() noexcept override;

    virtual void init();
    void startUiProcess();

private:
    void initModels();
    void initViewModels();
    void initViews();
    void setupApiConnections();
    void setupMVVMConnections();

protected:
    QApplication &m_app;

    // Network
    ApiManager &m_apiManager;

    // UI
    MainWindow *m_mainWindow;
    DialogManager *m_dialogManager;
};
