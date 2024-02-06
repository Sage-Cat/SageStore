#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include "Actions.hpp"

class MainMenu : public QMenuBar 
{
public:
        MainMenu();
    //destructor
    virtual ~MainMenu();

private slots:
    void showModuleAction(ActionTypes actionType);

public: //private
    QMenu* createModuleMenu(const QString& menuTitle, const std::vector<ActionTypes>& actions);
    
public: //private
    std::map<QAction*, ActionTypes> actionTypeMap;
    
    // Elements Menu
    QMenu *m_fileMenu,*m_purchasingMenu,*m_salesMenu,*m_inventoryMenu,*m_analyticsMenu,*m_usersMenu,*m_managementMenu; 

public slots:
    void showPurchaseOrders();
    void showSalesOrders();

};

//#endif 
