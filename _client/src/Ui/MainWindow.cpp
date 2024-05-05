#include "MainWindow.hpp"

#include <QVBoxLayout>

#include "Network/ApiManager.hpp"
#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/Views/BaseView.hpp"

#include "common/SpdlogConfig.hpp"

namespace
{
    constexpr int WINDOW_WIDTH = 1200;
    constexpr int WINDOW_HEIGHT = 800;
}

MainWindow::MainWindow(QApplication &app, ApiManager &apiClient, DialogManager &dialogManager, QWidget *parent)
    : QMainWindow(parent), m_app(app), m_apiManager(apiClient), m_dialogManager(dialogManager)
{
    m_tabWidget = new QTabWidget(this);
    m_statusBar = new QStatusBar(this);

    setupUi();
    setupMenu();
}

void MainWindow::startUiProcess()
{
    m_dialogManager.showLoginDialog();
    show();
}

void MainWindow::setupUi()
{
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    showMaximized();

    // MainMenu
    m_mainMenuBar = menuBar();
    connect(this, &MainWindow::menuActionTriggered, this, &MainWindow::handleMainMenuAction);

    // TabWidget
    m_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tabWidget->setTabsClosable(true);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, m_tabWidget, &QTabWidget::removeTab);

    // MainWindow layout
    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(m_tabWidget);
    layout->addWidget(m_statusBar, 0, Qt::AlignBottom);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // StatusBar
    setStatusBar(m_statusBar);
}

void MainWindow::setupMenu()
{
    // File Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("File"),
                                            {MainMenuActions::Type::SETTINGS,
                                             MainMenuActions::Type::EXIT}));
    // Purchasing Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Purchasing"),
                                            {MainMenuActions::Type::PURCHASE_ORDERS,
                                             MainMenuActions::Type::SUPPLIER_MANAGEMENT,
                                             MainMenuActions::Type::GOODS_RECEIPTS}));
    // Sales Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Sales"),
                                            {MainMenuActions::Type::SALES_ORDERS,
                                             MainMenuActions::Type::CUSTOMER_MANAGEMENT,
                                             MainMenuActions::Type::INVOICING}));
    // Inventory Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Inventory"),
                                            {MainMenuActions::Type::PRODUCT_MANAGEMENT,
                                             MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD,
                                             MainMenuActions::Type::STOCK_TRACKING}));
    // Analytics Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Analytics"),
                                            {MainMenuActions::Type::SALES_ANALYTICS,
                                             MainMenuActions::Type::INVENTORY_ANALYTICS}));
    // Users Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Users"),
                                            {MainMenuActions::Type::USER_ROLES,
                                             MainMenuActions::Type::USER_LOGS}));
    // Management Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Management"),
                                            {MainMenuActions::Type::EMPLOYEES,
                                             MainMenuActions::Type::CUSTOMERS,
                                             MainMenuActions::Type::SUPPLIERS}));
}

void MainWindow::handleMainMenuAction(MainMenuActions::Type actionType)
{
    SPDLOG_TRACE("MainWindow::setupMVVMConnections | actionType = {}",
                 MainMenuActions::NAMES.contains(actionType) ? MainMenuActions::NAMES.at(actionType) : std::to_string(static_cast<int>(actionType)));

    // Performing action based on actionType
    switch (actionType)
    {
    case MainMenuActions::Type::EXIT:
        close();
        break;
    default:
        SPDLOG_WARN("Unsupported MainMenuAction type.");
    }
}

QMenu *MainWindow::createModuleMenu(const QString &menuTitle, const QVector<MainMenuActions::Type> &actions)
{
    auto menu = new QMenu(menuTitle, this);
    for (auto actionType : actions)
    {
        auto action = new QAction(MainMenuActions::NAMES.at(actionType).c_str(), menu);
        connect(action, &QAction::triggered, [this, actionType]()
                { handleMainMenuAction(actionType); });
        menu->addAction(action);
    }
    return menu;
}