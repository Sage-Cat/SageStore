#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QString>
#include <QTabWidget>
#include <QVector>

#include "MainMenuActions.hpp"

#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Views/UsersView.hpp"

class ApiManager;
class DialogManager;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QApplication &app, ApiManager &apiClient, DialogManager &dialogManager,
                        QWidget *parent = nullptr);
    ~MainWindow() = default;

    void startUiProcess();

signals:
    void menuActionTriggered(MainMenuActions::Type actionType);

private: // --- General UI ---
    void setupUi();

    // MainMenu
    void setupMenu();
    QMenu *createModuleMenu(const QString &menuTitle,
                            const QVector<MainMenuActions::Type> &actions);
    void handleMainMenuAction(MainMenuActions::Type actionType);

private: // --- MVVM ---
    void setupMVVM();

private:
    QApplication &m_app;
    ApiManager &m_apiManager;
    DialogManager &m_dialogManager;

    /* UI */
    QTabWidget *m_tabWidget;
    QStatusBar *m_statusBar;
    QMenuBar *m_mainMenuBar;

    /* MVVM */
    // UsersManagement
    UsersManagementModel *m_usersManagementModel;
    UsersManagementViewModel *m_usersManagementViewModel;
    UsersView *m_usersView;
};
