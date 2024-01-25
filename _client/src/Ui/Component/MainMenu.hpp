#pragma once

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
private:
    QMenu* createModuleMenu(const QString& menuTitle, const std::vector<ActionTypes>& actions);
private:
    std::map<QAction*, ActionTypes> actionTypeMap;
    // File Menu
    QMenu *m_fileMenu;
    
    // Purchasing Module Menu
    QMenu *m_purchasingMenu;
    
    // Sales Module Menu
    QMenu *m_salesMenu;
    
    // Inventory Module Menu
    QMenu *m_inventoryMenu;
    
    // Analytics Module Menu
    QMenu *m_analyticsMenu;
    
    // Users Module Menu
    QMenu *m_usersMenu;
    
    // Management Module Menu
    QMenu *m_managementMenu;
    
public slots:
    void showPurchaseOrders();
    void showSalesOrders();

};

#endif 
