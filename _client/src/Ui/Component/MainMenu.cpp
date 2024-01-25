#include "MainMenu.hpp"
#include "Actions.hpp"

MainMenu::MainMenu() : QMenuBar()
{
    // File Menu
    m_fileMenu = createModuleMenu("File", {ActionTypes::SETTINGS, ActionTypes::EXIT}); 
        //QAction *settingsAction = new QAction("Settings", this);
        //QAction *exitAction = new QAction("Exit", this);
        // connect(exitAction, &QAction::triggered, this, &MainMenu::close);
    addMenu(m_fileMenu);

    // Purchasing Module Menu
    m_purchasingMenu = createModuleMenu("Purchasing", {ActionTypes::PURCHASE_ORDERS, ActionTypes::SUPPLIER_MANAGEMENT, ActionTypes::GOODS_RECEIPTS});
        //QAction *purchaseOrdersAction = new QAction("Purchase Orders", this);
        // connect(purchaseOrdersAction, &QAction::triggered, this, &MainMenu::showPurchaseOrders);
    addMenu(m_purchasingMenu);

    // Sales Module Menu
    m_salesMenu = createModuleMenu("Sales", {ActionTypes::SALES_ORDERS, ActionTypes::CUSTOMER_MANAGEMENT, ActionTypes::INVOICING});
        //QAction *salesOrdersAction = new QAction("Sales Orders", this);
        //  connect(salesOrdersAction, &QAction::triggered, this, &MainMenu::showSalesOrders);
    addMenu(m_salesMenu);

    // Inventory Module Menu
    m_inventoryMenu = createModuleMenu("Inventory", {ActionTypes::PRODUCT_MANAGEMENT, ActionTypes::SUPPLIER_PRICELIST_UPLOAD, ActionTypes::STOCK_TRACKING});
    addMenu(m_inventoryMenu);

    // Analytics Module Menu
    m_analyticsMenu = createModuleMenu("Analytics", {ActionTypes::SALES_ANALYTICS, ActionTypes::INVENTORY_ANALYTICS});
    addMenu(m_analyticsMenu);

    // Users Module Menu
    m_usersMenu = createModuleMenu("Users", {ActionTypes::USER_ROLES, ActionTypes::USER_LOGS});
    addMenu(m_usersMenu);

    // Management Module Menu
    m_managementMenu = createModuleMenu("Management", {ActionTypes::EMPLOYEES, ActionTypes::CUSTOMERS, ActionTypes::SUPPLIERS});
    addMenu(m_managementMenu);

}
//Custom function that create module menu using map
QMenu* MainMenu::createModuleMenu(const QString& menuTitle, const std::vector<ActionTypes>& actions)
{
    QMenu* menu = new QMenu(menuTitle);

    for (ActionTypes actionType : actions) {
        QAction* action = new QAction(ACTION_NAMES.at(actionType).c_str(), this);
        connect(action, &QAction::triggered, this, [this, actionType]() { showModuleAction(actionType); });
        menu->addAction(action);

        actionTypeMap[action] = actionType;
    }

    return menu;
}

