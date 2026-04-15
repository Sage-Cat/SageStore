#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QString>
#include <QTabWidget>
#include <QVector>

#include "MainMenuActions.hpp"

#include "Ui/Models/AnalyticsModel.hpp"
#include "Ui/Models/LogsModel.hpp"
#include "Ui/Models/ManagementModel.hpp"
#include "Ui/Models/ProductTypesModel.hpp"
#include "Ui/Models/PurchaseModel.hpp"
#include "Ui/Models/SalesModel.hpp"
#include "Ui/Models/SupplierCatalogModel.hpp"
#include "Ui/Models/StocksModel.hpp"
#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/ViewModels/AnalyticsViewModel.hpp"
#include "Ui/ViewModels/LogsViewModel.hpp"
#include "Ui/ViewModels/ManagementViewModel.hpp"
#include "Ui/ViewModels/ProductTypesViewModel.hpp"
#include "Ui/ViewModels/PurchaseViewModel.hpp"
#include "Ui/ViewModels/SalesViewModel.hpp"
#include "Ui/ViewModels/SupplierCatalogViewModel.hpp"
#include "Ui/ViewModels/StocksViewModel.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "Ui/Views/AnalyticsView.hpp"
#include "Ui/Views/LogsView.hpp"
#include "Ui/Views/ManagementView.hpp"
#include "Ui/Views/ProductTypesView.hpp"
#include "Ui/Views/PurchaseView.hpp"
#include "Ui/Views/RolesView.hpp"
#include "Ui/Views/SalesView.hpp"
#include "Ui/Views/SettingsView.hpp"
#include "Ui/Views/StocksView.hpp"
#include "Ui/Views/SupplierCatalogView.hpp"
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
    void ensureUsersManagementState();
    ProductTypesView *ensureProductTypesView();
    StocksView *ensureStocksView();
    UsersView *ensureUsersView();
    RolesView *ensureRolesView();
    SettingsView *ensureSettingsView();
    ManagementView *ensureManagementView();
    SupplierCatalogView *ensureSupplierCatalogView();
    PurchaseView *ensurePurchaseView();
    SalesView *ensureSalesView();
    LogsView *ensureLogsView();
    AnalyticsView *ensureAnalyticsView();

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
    RolesView *m_rolesView{nullptr};
    SettingsView *m_settingsView{nullptr};

    // Management
    ManagementModel *m_managementModel{nullptr};
    ManagementViewModel *m_managementViewModel{nullptr};
    ManagementView *m_managementView{nullptr};

    // Supplier catalog
    SupplierCatalogModel *m_supplierCatalogModel{nullptr};
    SupplierCatalogViewModel *m_supplierCatalogViewModel{nullptr};
    SupplierCatalogView *m_supplierCatalogView{nullptr};

    // Purchase
    PurchaseModel *m_purchaseModel{nullptr};
    PurchaseViewModel *m_purchaseViewModel{nullptr};
    PurchaseView *m_purchaseView{nullptr};

    // Sales
    SalesModel *m_salesModel{nullptr};
    SalesViewModel *m_salesViewModel{nullptr};
    SalesView *m_salesView{nullptr};

    // Logs
    LogsModel *m_logsModel{nullptr};
    LogsViewModel *m_logsViewModel{nullptr};
    LogsView *m_logsView{nullptr};

    // Analytics
    AnalyticsModel *m_analyticsModel{nullptr};
    AnalyticsViewModel *m_analyticsViewModel{nullptr};
    AnalyticsView *m_analyticsView{nullptr};
};
