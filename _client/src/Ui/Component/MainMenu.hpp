#ifndef MAINMENU_H
#define MAINMENU_H

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>

class MainMenu : public QMenuBar
{
public:
    MainMenu();
    //destructor
    virtual ~MainMenu();

    // File Menu
    QMenu *fileMenu = this->addMenu("File");
    QAction *settingsAction = new QAction("Settings", this);
    QAction *exitAction = new QAction("Exit", this);
    // Purchasing Module Menu
    QMenu *purchasingMenu = this->addMenu("Purchasing");
    QAction *purchaseOrdersAction = new QAction("Purchase Orders", this);
    // Sales Module Menu
    QMenu *salesMenu = this->addMenu("Sales");
    QAction *salesOrdersAction = new QAction("Sales Orders", this);
    // Inventory Module Menu
    QMenu *inventoryMenu = this->addMenu("Inventory");
    // Analytics Module Menu
    QMenu *analyticsMenu = this->addMenu("Analytics");
    // Users Module Menu
    QMenu *usersMenu = this->addMenu("Users");
    // Management Module Menu
    QMenu *managementMenu = this->addMenu("Management");
public slots:
    void showPurchaseOrders();
    void showSalesOrders();

};

#endif // MAINMENU_H
