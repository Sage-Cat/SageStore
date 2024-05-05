#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QString>
#include <QVector>

#include "MainMenuActions.hpp"

class ApiManager;
class DialogManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QApplication &app,
                        ApiManager &apiClient,
                        DialogManager &dialogManager,
                        QWidget *parent = nullptr);
    ~MainWindow() = default;

    void startUiProcess();

signals:
    void menuActionTriggered(MainMenuActions::Type actionType);

private:
    void setupUi();

    // MainMenu
    void setupMenu();
    QMenu *createModuleMenu(const QString &menuTitle, const QVector<MainMenuActions::Type> &actions);
    void handleMainMenuAction(MainMenuActions::Type actionType);

private:
    QApplication &m_app;
    ApiManager &m_apiManager;
    DialogManager &m_dialogManager;

    // Ui
    QTabWidget *m_tabWidget;
    QStatusBar *m_statusBar;
    QMenuBar *m_mainMenuBar;
};
