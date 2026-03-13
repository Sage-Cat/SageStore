#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QString>
#include <QTabWidget>
#include <QVector>

#include "MainMenuActions.hpp"

#include "Ui/Models/ProductTypesModel.hpp"
#include "Ui/Models/StocksModel.hpp"
#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/ViewModels/ProductTypesViewModel.hpp"
#include "Ui/ViewModels/StocksViewModel.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Views/ProductTypesView.hpp"
#include "Ui/Views/StocksView.hpp"
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
    ProductTypesView *ensureProductTypesView();
    StocksView *ensureStocksView();
    UsersView *ensureUsersView();

private:
    QApplication &m_app;
    ApiManager &m_apiManager;
    DialogManager &m_dialogManager;

    /* UI */
    QTabWidget *m_tabWidget;
    QStatusBar *m_statusBar;
    QMenuBar *m_mainMenuBar;

    /* MVVM */
    // Product types
    ProductTypesModel *m_productTypesModel{nullptr};
    ProductTypesViewModel *m_productTypesViewModel{nullptr};
    ProductTypesView *m_productTypesView{nullptr};

    // Stocks
    StocksModel *m_stocksModel{nullptr};
    StocksViewModel *m_stocksViewModel{nullptr};
    StocksView *m_stocksView{nullptr};

    // UsersManagement
    UsersManagementModel *m_usersManagementModel{nullptr};
    UsersManagementViewModel *m_usersManagementViewModel{nullptr};
    UsersView *m_usersView{nullptr};
};
