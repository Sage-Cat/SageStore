#include "MainMenuView.hpp"
#include "Actions.hpp"

MainMenu::MainMenu() : QMenuBar()
{
    // File Menu
    m_fileMenu = createModuleMenu(tr("File"), {ActionTypes::SETTINGS, ActionTypes::EXIT}); 
        //QAction *settingsAction = new QAction("Settings", this);
        //QAction *exitAction = new QAction("Exit", this);
        // connect(exitAction, &QAction::triggered, this, &MainMenu::close);
    addMenu(m_fileMenu);

    // Purchasing Module Menu
    m_purchasingMenu = createModuleMenu(tr("Purchasing"), {ActionTypes::PURCHASE_ORDERS, ActionTypes::SUPPLIER_MANAGEMENT, ActionTypes::GOODS_RECEIPTS});
        //QAction *purchaseOrdersAction = new QAction("Purchase Orders", this);
        // connect(purchaseOrdersAction, &QAction::triggered, this, &MainMenu::showPurchaseOrders);
    addMenu(m_purchasingMenu);

    // Sales Module Menu
    m_salesMenu = createModuleMenu(tr("Sales"), {ActionTypes::SALES_ORDERS, ActionTypes::CUSTOMER_MANAGEMENT, ActionTypes::INVOICING});
        //QAction *salesOrdersAction = new QAction("Sales Orders", this);
        //  connect(salesOrdersAction, &QAction::triggered, this, &MainMenu::showSalesOrders);
    addMenu(m_salesMenu);

    // Inventory Module Menu
    m_inventoryMenu = createModuleMenu(tr("Inventory"), {ActionTypes::PRODUCT_MANAGEMENT, ActionTypes::SUPPLIER_PRICELIST_UPLOAD, ActionTypes::STOCK_TRACKING});
    addMenu(m_inventoryMenu);

    // Analytics Module Menu
    m_analyticsMenu = createModuleMenu(tr("Analytics"), {ActionTypes::SALES_ANALYTICS, ActionTypes::INVENTORY_ANALYTICS});
    addMenu(m_analyticsMenu);

    // Users Module Menu
    m_usersMenu = createModuleMenu(tr("Users"), {ActionTypes::USER_ROLES, ActionTypes::USER_LOGS});
    addMenu(m_usersMenu);

    // Management Module Menu
    m_managementMenu = createModuleMenu(tr("Management"), {ActionTypes::EMPLOYEES, ActionTypes::CUSTOMERS, ActionTypes::SUPPLIERS});
    addMenu(m_managementMenu);

}
void MainMenu::showModuleAction(ActionTypes actionType)
{
    qDebug() << "Show " << ACTION_NAMES.at(actionType).c_str();
}



