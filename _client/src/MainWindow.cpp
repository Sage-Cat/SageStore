#include "MainWindow.hpp"

#include "SpdlogWrapper.hpp"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

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
    m_stackView->resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // add status bar
    m_statusBar = new QStatusBar(this);
    this->setStatusBar(m_statusBar);
}

void MainWindow::setupUI()
{
    // set size and at fullscreen
    this->resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    this->showMaximized();
}

void MainWindow::setupMenu() 
{
    // create MenuBar
    m_mainMenuBar =  menuBar();

    // File Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("File"), {ActionTypes::SETTINGS, ActionTypes::EXIT}));
    
    // Purchasing Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Purchasing"), {ActionTypes::PURCHASE_ORDERS, ActionTypes::SUPPLIER_MANAGEMENT, ActionTypes::GOODS_RECEIPTS}));

    // Sales Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Sales"), {ActionTypes::SALES_ORDERS, ActionTypes::CUSTOMER_MANAGEMENT, ActionTypes::INVOICING}));
    
    // Inventory Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Inventory"), {ActionTypes::PRODUCT_MANAGEMENT, ActionTypes::SUPPLIER_PRICELIST_UPLOAD, ActionTypes::STOCK_TRACKING}));

    // Analytics Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Analytics"), {ActionTypes::SALES_ANALYTICS, ActionTypes::INVENTORY_ANALYTICS}));

    // Users Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Users"), {ActionTypes::USER_ROLES, ActionTypes::USER_LOGS}));

    // Management Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Management"), {ActionTypes::EMPLOYEES, ActionTypes::CUSTOMERS, ActionTypes::SUPPLIERS}));
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

