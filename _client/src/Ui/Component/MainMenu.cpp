#include "MainMenu.hpp"

MainMenu::MainMenu() : QMenuBar()
{
    // File Menu
    fileMenu->addAction(settingsAction);
    connect(exitAction, &QAction::triggered, this, &MainMenu::close);
    fileMenu->addAction(exitAction);

    // Purchasing Module Menu
    connect(purchaseOrdersAction, &QAction::triggered, this, &MainMenu::showPurchaseOrders);
    purchasingMenu->addAction(purchaseOrdersAction);
    purchasingMenu->addAction("Supplier Management");
    purchasingMenu->addAction("Goods Receipts");

    // Sales Module Menu
    connect(salesOrdersAction, &QAction::triggered, this, &MainMenu::showSalesOrders);
    salesMenu->addAction(salesOrdersAction);
    salesMenu->addAction("Customer Management");
    salesMenu->addAction("Invoicing");

    // Inventory Module Menu
    inventoryMenu->addAction("Product Management");
    inventoryMenu->addAction("Supplier's Pricelist Upload");
    inventoryMenu->addAction("Stock Tracking");

    // Analytics Module Menu
    analyticsMenu->addAction("Sales Analytics");
    analyticsMenu->addAction("Inventory Analytics");

    // Users Module Menu
    usersMenu->addAction("User Roles");
    usersMenu->addAction("User Logs");

    // Management Module Menu
    managementMenu->addAction("Employees");
    managementMenu->addAction("Customers");
    managementMenu->addAction("Suppliers");

}

void MainMenu::showPurchaseOrders()
{
    qDebug() << "Show Purchase Orders";
}

void MainMenu::showSalesOrders()
{
    qDebug() << "Show Sales Orders";
}
