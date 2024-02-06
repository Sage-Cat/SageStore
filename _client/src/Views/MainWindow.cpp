#include "MainWindow.hpp"
#include "MainMenuView.hpp"
#include "SpdlogWrapper.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{   
    init();
    setupUI();  
    setupMenu();
}

void MainWindow::init()
{
    // add stackView
    m_stackView = new QStackedWidget(this);
    m_stackView->resize(MainWindowSize::WINDOW_WIDTH, MainWindowSize::WINDOW_HEIGHT);

    // add status bar
    m_statusBar = new QStatusBar(this);
    this->setStatusBar(m_statusBar);
}

void MainWindow::setupUI()
{
    // set size and at fullscreen
    this->resize(MainWindowSize::WINDOW_WIDTH, MainWindowSize::WINDOW_HEIGHT);
    this->showMaximized();
}

void MainWindow::setupMenu() 
{
    // create MenuBar
    m_mainMenuBar =  menuBar();
    // File Menu
    m_fileMenu = createModuleMenu(tr("File"), {ActionTypes::SETTINGS, ActionTypes::EXIT}); 
    m_mainMenuBar->addMenu(m_fileMenu);

    // Purchasing Module Menu
    m_purchasingMenu = createModuleMenu(tr("Purchasing"), {ActionTypes::PURCHASE_ORDERS, ActionTypes::SUPPLIER_MANAGEMENT, ActionTypes::GOODS_RECEIPTS});
    m_mainMenuBar->addMenu(m_purchasingMenu);

    // Sales Module Menu
    m_salesMenu = createModuleMenu(tr("Sales"), {ActionTypes::SALES_ORDERS, ActionTypes::CUSTOMER_MANAGEMENT, ActionTypes::INVOICING});
   m_mainMenuBar->addMenu(m_salesMenu);

    // Inventory Module Menu
    m_inventoryMenu = createModuleMenu(tr("Inventory"), {ActionTypes::PRODUCT_MANAGEMENT, ActionTypes::SUPPLIER_PRICELIST_UPLOAD, ActionTypes::STOCK_TRACKING});
    m_mainMenuBar->addMenu(m_inventoryMenu);

    // Analytics Module Menu
    m_analyticsMenu = createModuleMenu(tr("Analytics"), {ActionTypes::SALES_ANALYTICS, ActionTypes::INVENTORY_ANALYTICS});
    m_mainMenuBar->addMenu(m_analyticsMenu);

    // Users Module Menu
    m_usersMenu = createModuleMenu(tr("Users"), {ActionTypes::USER_ROLES, ActionTypes::USER_LOGS});
    m_mainMenuBar->addMenu(m_usersMenu);

    // Management Module Menu
    m_managementMenu = createModuleMenu(tr("Management"), {ActionTypes::EMPLOYEES, ActionTypes::CUSTOMERS, ActionTypes::SUPPLIERS});
    m_mainMenuBar->addMenu(m_managementMenu);
}
//Custom function that create module menu using map
QMenu* MainWindow::createModuleMenu(const QString& menuTitle, const std::vector<ActionTypes>& actions)
{
    QMenu* menu = new QMenu(menuTitle);

    for (ActionTypes actionType : actions) {
        QAction* action = new QAction(ACTION_NAMES.at(actionType).c_str(), this);
        connect(action, &QAction::triggered, this, [this, actionType]() {SPDLOG_DEBUG(std::string("Show ") + ACTION_NAMES.at(actionType).c_str());});
        menu->addAction(action);
    }

    return menu;
}
MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_stackView->resize(this->size().width(), this->size().height());
}

