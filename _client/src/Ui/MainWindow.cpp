#include "MainWindow.hpp"

#include "SpdlogConfig.hpp"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
}

void MainWindow::init()
{
    // add stackView
    m_stackView = new QStackedWidget(this);
    m_stackView->resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // add status bar
    m_statusBar = new QStatusBar(this);
    this->setStatusBar(m_statusBar);

    // setup connections
    setupUi();
    setupMenu();
}

void MainWindow::setupUi()
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
    m_mainMenuBar->addMenu(createModuleMenu(tr("File"), {Actions::Types::SETTINGS, Actions::Types::EXIT}));
    
    // Purchasing Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Purchasing"), {Actions::Types::PURCHASE_ORDERS, Actions::Types::SUPPLIER_MANAGEMENT, Actions::Types::GOODS_RECEIPTS}));

    // Sales Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Sales"), {Actions::Types::SALES_ORDERS, Actions::Types::CUSTOMER_MANAGEMENT, Actions::Types::INVOICING}));
    
    // Inventory Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Inventory"), {Actions::Types::PRODUCT_MANAGEMENT, Actions::Types::SUPPLIER_PRICELIST_UPLOAD, Actions::Types::STOCK_TRACKING}));

    // Analytics Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Analytics"), {Actions::Types::SALES_ANALYTICS, Actions::Types::INVENTORY_ANALYTICS}));

    // Users Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Users"), {Actions::Types::USER_ROLES, Actions::Types::USER_LOGS}));

    // Management Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Management"), {Actions::Types::EMPLOYEES, Actions::Types::CUSTOMERS, Actions::Types::SUPPLIERS}));
}

//Custom function that create module menu using map
QMenu* MainWindow::createModuleMenu(const QString& menuTitle, const std::vector<Actions::Types>& names)
{
    QMenu* menu = new QMenu(menuTitle, this);

    for (Actions::Types actionType : names) {
        QAction* action = new QAction(Actions::NAMES.at(actionType).c_str(), menu);
        connect(action, &QAction::triggered, menu, [menu, actionType]() {SPDLOG_DEBUG(std::string("Show ") + Actions::NAMES.at(actionType).c_str());});
        menu->addAction(action);
    }
    return menu;
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    m_stackView->resize(this->size().width(), this->size().height());
}

