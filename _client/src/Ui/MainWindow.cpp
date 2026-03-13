#include "MainWindow.hpp"

#include <QAction>
#include <QMenu>
#include <QScreen>
#include <QVBoxLayout>

#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainMenuActions.hpp"
#include "Ui/UiScale.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
const QSize BASE_WINDOW_SIZE{1200, 800};

struct ModuleStatusSpec {
    QString statusBadge;
    QString summary;
    ModuleStatusView::Section currentState;
    ModuleStatusView::Section nextSteps;
};

ModuleStatusSpec buildModuleStatusSpec(MainMenuActions::Type actionType)
{
    using Type = MainMenuActions::Type;

    switch (actionType) {
    case Type::SETTINGS:
        return {.statusBadge = QStringLiteral("Current Baseline"),
                .summary = QStringLiteral(
                    "Runtime configuration is intentionally simple: desktop and server endpoints "
                    "are environment-driven today, which keeps the current system consumer-agnostic "
                    "without adding a premature settings editor."),
                .currentState =
                    {.title = QStringLiteral("Available Today"),
                     .items = {QStringLiteral(
                                   "Client and server host/port values can be overridden through "
                                   "environment variables."),
                               QStringLiteral(
                                   "Smoke scripts and the deployment runbook document the current "
                                   "bootstrap flow.")}},
                .nextSteps =
                    {.title = QStringLiteral("Next Sensible Step"),
                     .items = {QStringLiteral(
                                   "Add a persisted settings UX only when cross-client "
                                   "configuration requirements are defined."),
                               QStringLiteral(
                                   "Keep configuration transport-neutral so desktop is not the only "
                                   "supported consumer.")}}};
    case Type::PURCHASE_ORDERS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Purchase Orders are part of the documented ERP target, but no validated "
                    "purchase-order resource or desktop CRUD flow exists yet in this repository."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Inventory foundations are implemented through product types and "
                                   "stock tracking."),
                               QStringLiteral(
                                   "There is no end-to-end purchase-order backend or Qt workflow "
                                   "yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Define purchase order header and line-item contracts."),
                               QStringLiteral(
                                   "Implement repository, business module, HTTP contract, and "
                                   "desktop CRUD as one vertical slice.")}}};
    case Type::SUPPLIER_MANAGEMENT:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Supplier management is documented, but it is not yet backed by a validated "
                    "server resource or desktop CRUD workflow."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Supplier terminology exists in product documentation and menu "
                                   "structure."),
                               QStringLiteral(
                                   "No supplier entity lifecycle is implemented end-to-end.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Normalize supplier data model and relations to product types "
                                   "and purchase orders."),
                               QStringLiteral(
                                   "Add supplier CRUD before bulk price-list upload workflows.")}}};
    case Type::GOODS_RECEIPTS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Goods Receipts depends on transactional purchasing flows that are not yet "
                    "implemented in the current repository baseline."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Stock CRUD exists, but it is not tied to receipt documents or "
                                   "order fulfillment."),
                               QStringLiteral(
                                   "No receipt posting or reconciliation flow is validated yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Introduce purchase receipt records linked to purchase orders."),
                               QStringLiteral(
                                   "Post stock changes through document-backed workflows rather "
                                   "than only direct stock edits.")}}};
    case Type::SALES_ORDERS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Sales Orders remain part of the target-state ERP scope, but there is no "
                    "validated sales order slice in code yet."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Users, roles, product types, and stock records are available "
                                   "as baseline foundations."),
                               QStringLiteral(
                                   "No sales-order repository, business logic, or desktop CRUD is "
                                   "implemented.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Define sales order header and line-item contracts."),
                               QStringLiteral(
                                   "Add contract-tested CRUD before invoice export or analytics.")}}};
    case Type::CUSTOMER_MANAGEMENT:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Customer management is still at the documentation stage. Terminology across "
                    "customer, client, and contact is being normalized before implementation."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "The product docs describe customers/contacts as future "
                                   "business actors."),
                               QStringLiteral(
                                   "No validated customer resource or UI exists in code yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Choose one canonical customer/contact model."),
                               QStringLiteral(
                                   "Implement CRUD once sales and management ownership boundaries "
                                   "are agreed.")}}};
    case Type::INVOICING:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Invoicing is a target module, but the current repository does not yet "
                    "contain a validated invoice resource, export flow, or desktop workflow."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "No invoice serializer, repository, or contract-tested API is "
                                   "implemented."),
                               QStringLiteral(
                                   "Current inventory and users slices provide only supporting "
                                   "foundations.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Build invoice CRUD after the base sales transaction model "
                                   "exists."),
                               QStringLiteral(
                                   "Add export behavior only after the core invoice lifecycle is "
                                   "validated.")}}};
    case Type::SUPPLIER_PRICELIST_UPLOAD:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Supplier price-list upload is intentionally held back until supplier data and "
                    "import contracts exist end-to-end."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Product types can be maintained manually today."),
                               QStringLiteral(
                                   "There is no validated supplier import pipeline or mapping UI "
                                   "yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Implement supplier CRUD and import file contract first."),
                               QStringLiteral(
                                   "Add review/mapping UX before bulk catalog mutation is allowed.")}}};
    case Type::SALES_ANALYTICS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Sales analytics should be derived from a stable sales data model. That "
                    "transactional base is not implemented yet, so the analytics surface remains "
                    "planned."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "There is no validated sales dataset to aggregate."),
                               QStringLiteral(
                                   "Analytics menus are present so scope stays visible, but the "
                                   "repo is not pretending the slice exists.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Complete base sales CRUD and invoice flows first."),
                               QStringLiteral(
                                   "Add read-only reporting endpoints and dashboards afterward.")}}};
    case Type::INVENTORY_ANALYTICS:
        return {.statusBadge = QStringLiteral("Foundation Ready"),
                .summary = QStringLiteral(
                    "Inventory analytics is not a dedicated slice yet, but it can build directly "
                    "on the implemented product type and stock foundations."),
                .currentState =
                    {.title = QStringLiteral("Available Today"),
                     .items = {QStringLiteral(
                                   "Product Management and Stock Tracking are implemented and "
                                   "validated."),
                               QStringLiteral(
                                   "No aggregate inventory reporting API or dashboard exists yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Add low-cost stock summary and threshold report endpoints."),
                               QStringLiteral(
                                   "Keep analytics read-only and consumer-agnostic.")}}};
    case Type::USER_LOGS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "User Logs are documented and partially reflected in schema placeholders, but "
                    "there is still no validated audit-log API or browsing UI."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "The repo records Logs as planned only."),
                               QStringLiteral(
                                   "No end-to-end activity-log implementation is wired today.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Define audit event schema and retention expectations."),
                               QStringLiteral(
                                   "Implement read-only log browsing before advanced filtering or "
                                   "analytics.")}}};
    case Type::EMPLOYEES:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Employees remain a documented management module, but there is no validated "
                    "employee CRUD slice in the current codebase."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Stock records currently reference employee IDs only as simple "
                                   "values."),
                               QStringLiteral(
                                   "There is no employee master-data resource or UI yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Introduce employee master data before expanding inventory "
                                   "responsibility flows."),
                               QStringLiteral(
                                   "Keep employee data separate from authentication users unless "
                                   "the business rules require linking them.")}}};
    case Type::CUSTOMERS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Customers is a planned management/master-data view. It is not yet backed by "
                    "a validated server resource."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Customer terminology exists in docs and menu structure."),
                               QStringLiteral(
                                   "No repository, endpoint, or desktop CRUD workflow exists yet.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Unify the customer/contact/client naming decision."),
                               QStringLiteral(
                                   "Implement CRUD once the sales slice needs it as shared master "
                                   "data.")}}};
    case Type::SUPPLIERS:
        return {.statusBadge = QStringLiteral("Planned Slice"),
                .summary = QStringLiteral(
                    "Suppliers is the management/master-data counterpart to purchasing and catalog "
                    "import. It is still a planned surface."),
                .currentState =
                    {.title = QStringLiteral("Current Repo Support"),
                     .items = {QStringLiteral(
                                   "Supplier-facing menus exist so the target scope remains "
                                   "visible."),
                               QStringLiteral(
                                   "No validated supplier CRUD flow is implemented end-to-end.")}},
                .nextSteps =
                    {.title = QStringLiteral("Recommended Implementation"),
                     .items = {QStringLiteral(
                                   "Introduce supplier master data and reuse it across purchasing "
                                   "and price-list import."),
                               QStringLiteral(
                                   "Keep the server contract resource-oriented for future non-Qt "
                                   "consumers.")}}};
    case Type::EXIT:
    case Type::PRODUCT_MANAGEMENT:
    case Type::STOCK_TRACKING:
    case Type::USERS:
    case Type::USER_ROLES:
        break;
    }

    return {.statusBadge = QStringLiteral("Unavailable"),
            .summary = QStringLiteral("This menu item is not configured."),
            .currentState = {.title = QStringLiteral("Current Repo Support"), .items = {}},
            .nextSteps = {.title = QStringLiteral("Recommended Implementation"), .items = {}}};
}
} // namespace

MainWindow::MainWindow(QApplication &app, ApiManager &apiClient, DialogManager &dialogManager,
                       QWidget *parent)
    : QMainWindow(parent), m_app(app), m_apiManager(apiClient), m_dialogManager(dialogManager),
      m_tabWidget(new QTabWidget(this)), m_statusBar(new QStatusBar(this))
{
    setupUi();
    setupMenu();
    setupMVVM();
}

void MainWindow::startUiProcess()
{
    m_dialogManager.showLoginDialog();
    show();
}

void MainWindow::setupUi()
{
    const QScreen *primaryScreen = m_app.primaryScreen();
    const QRect availableGeometry =
        primaryScreen ? primaryScreen->availableGeometry() : QRect();

    resize(UiScale::scaledWindowSize(BASE_WINDOW_SIZE, availableGeometry));
    setMinimumSize(UiScale::scalePx(960), UiScale::scalePx(640));

    if (availableGeometry.isValid()) {
        const QPoint centeredPosition =
            availableGeometry.center() - QPoint(width() / 2, height() / 2);
        move(centeredPosition);
    }

    // MainMenu
    m_mainMenuBar = menuBar();
    connect(this, &MainWindow::menuActionTriggered, this, &MainWindow::handleMainMenuAction);

    // TabWidget
    m_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_tabWidget->setTabsClosable(true);
    m_tabWidget->setDocumentMode(true);
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

    setStyleSheet(R"(
        QMainWindow { background-color: #f4efe7; }
        QMenuBar { background-color: #34504f; color: #f5efe6; }
        QMenuBar::item { padding: 6px 10px; background: transparent; }
        QMenuBar::item:selected { background: #496a68; border-radius: 4px; }
        QMenu { background-color: #fffaf2; border: 1px solid #d8cdbd; }
        QMenu::item:selected { background-color: #e3d5bd; }
        QTabWidget::pane {
            border: 1px solid #d8cdbd;
            background-color: #fffaf2;
            border-radius: 8px;
            top: -1px;
        }
        QTabBar::tab {
            background: #d9cfbf;
            color: #2f3736;
            padding: 8px 14px;
            margin-right: 4px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
        }
        QTabBar::tab:selected { background: #fffaf2; }
        QStatusBar { background: #e9dfd1; color: #4a433b; }
    )");
}

void MainWindow::setupMenu()
{
    // File Menu
    m_mainMenuBar->addMenu(createModuleMenu(
        tr("File"), {MainMenuActions::Type::SETTINGS, MainMenuActions::Type::EXIT}));
    // Purchasing Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Purchasing"), {MainMenuActions::Type::PURCHASE_ORDERS,
                                            MainMenuActions::Type::SUPPLIER_MANAGEMENT,
                                            MainMenuActions::Type::GOODS_RECEIPTS}));
    // Sales Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Sales"), {MainMenuActions::Type::SALES_ORDERS,
                                       MainMenuActions::Type::CUSTOMER_MANAGEMENT,
                                       MainMenuActions::Type::INVOICING}));
    // Inventory Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Inventory"), {MainMenuActions::Type::PRODUCT_MANAGEMENT,
                                           MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD,
                                           MainMenuActions::Type::STOCK_TRACKING}));
    // Analytics Module Menu
    m_mainMenuBar->addMenu(
        createModuleMenu(tr("Analytics"), {MainMenuActions::Type::SALES_ANALYTICS,
                                           MainMenuActions::Type::INVENTORY_ANALYTICS}));
    // Users Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Users"), {MainMenuActions::Type::USERS,
                                                          MainMenuActions::Type::USER_ROLES,
                                                          MainMenuActions::Type::USER_LOGS}));
    // Management Module Menu
    m_mainMenuBar->addMenu(createModuleMenu(tr("Management"), {MainMenuActions::Type::EMPLOYEES,
                                                               MainMenuActions::Type::CUSTOMERS,
                                                               MainMenuActions::Type::SUPPLIERS}));
}

void MainWindow::handleMainMenuAction(MainMenuActions::Type actionType)
{
    SPDLOG_TRACE("MainWindow::setupMVVMConnections | actionType = {}",
                 MainMenuActions::NAMES.contains(actionType)
                     ? MainMenuActions::NAMES.at(actionType).toStdString()
                     : std::to_string(static_cast<int>(actionType)));

    const auto openTab = [this](QWidget *widget, MainMenuActions::Type type) {
        const auto title = MainMenuActions::NAMES.at(type);
        auto tabIndex    = m_tabWidget->indexOf(widget);
        const bool willOpenTab = tabIndex == -1;
        if (tabIndex == -1) {
            tabIndex = m_tabWidget->addTab(widget, title);
        }
        m_tabWidget->setCurrentIndex(tabIndex);
        return willOpenTab;
    };

    // Performing action based on actionType
    switch (actionType) {
    case MainMenuActions::Type::SETTINGS:
    case MainMenuActions::Type::PURCHASE_ORDERS:
    case MainMenuActions::Type::SUPPLIER_MANAGEMENT:
    case MainMenuActions::Type::GOODS_RECEIPTS:
    case MainMenuActions::Type::SALES_ORDERS:
    case MainMenuActions::Type::CUSTOMER_MANAGEMENT:
    case MainMenuActions::Type::INVOICING:
    case MainMenuActions::Type::SUPPLIER_PRICELIST_UPLOAD:
    case MainMenuActions::Type::SALES_ANALYTICS:
    case MainMenuActions::Type::INVENTORY_ANALYTICS:
    case MainMenuActions::Type::USER_LOGS:
    case MainMenuActions::Type::EMPLOYEES:
    case MainMenuActions::Type::CUSTOMERS:
    case MainMenuActions::Type::SUPPLIERS: {
        auto *view = ensureModuleStatusView(actionType);
        openTab(view, actionType);
        break;
    }
    case MainMenuActions::Type::PRODUCT_MANAGEMENT: {
        auto *view = ensureProductTypesView();
        if (openTab(view, MainMenuActions::Type::PRODUCT_MANAGEMENT)) {
            m_productTypesViewModel->fetchProductTypes();
        }
        break;
    }
    case MainMenuActions::Type::STOCK_TRACKING: {
        auto *view = ensureStocksView();
        if (openTab(view, MainMenuActions::Type::STOCK_TRACKING)) {
            m_stocksViewModel->fetchProductTypes();
            m_stocksViewModel->fetchStocks();
        }
        break;
    }
    case MainMenuActions::Type::USERS: {
        auto *view = ensureUsersView();
        if (openTab(view, MainMenuActions::Type::USERS)) {
            m_usersManagementViewModel->fetchRoles();
            m_usersManagementViewModel->fetchUsers();
        }
        break;
    }
    case MainMenuActions::Type::USER_ROLES: {
        auto *view = ensureRolesView();
        if (openTab(view, MainMenuActions::Type::USER_ROLES)) {
            m_usersManagementViewModel->fetchRoles();
        }
        break;
    }
    case MainMenuActions::Type::EXIT:
        close();
        break;
    }
}

QMenu *MainWindow::createModuleMenu(const QString &menuTitle,
                                    const QVector<MainMenuActions::Type> &actions)
{
    auto menu = new QMenu(menuTitle, this);
    for (auto actionType : actions) {
        auto action = new QAction(MainMenuActions::NAMES.at(actionType), menu);
        connect(action, &QAction::triggered,
                [this, actionType]() { emit menuActionTriggered(actionType); });
        menu->addAction(action);
    }
    return menu;
}

void MainWindow::setupMVVM()
{
    // Views are created lazily when the corresponding tab is first opened.
}

void MainWindow::ensureUsersManagementState()
{
    if (m_usersManagementModel == nullptr) {
        m_usersManagementModel = new UsersManagementModel(m_apiManager, this);
    }

    if (m_usersManagementViewModel == nullptr) {
        m_usersManagementViewModel = new UsersManagementViewModel(*m_usersManagementModel, this);
        connect(m_usersManagementViewModel, &UsersManagementViewModel::errorOccurred,
                &m_dialogManager, &DialogManager::showErrorDialog);
    }
}

ProductTypesView *MainWindow::ensureProductTypesView()
{
    if (m_productTypesView != nullptr) {
        return m_productTypesView;
    }

    m_productTypesModel     = new ProductTypesModel(m_apiManager, this);
    m_productTypesViewModel = new ProductTypesViewModel(*m_productTypesModel, this);
    m_productTypesView      = new ProductTypesView(*m_productTypesViewModel, this);
    connect(m_productTypesViewModel, &ProductTypesViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_productTypesView;
}

StocksView *MainWindow::ensureStocksView()
{
    if (m_stocksView != nullptr) {
        return m_stocksView;
    }

    m_stocksModel     = new StocksModel(m_apiManager, this);
    m_stocksViewModel = new StocksViewModel(*m_stocksModel, this);
    m_stocksView      = new StocksView(*m_stocksViewModel, this);
    connect(m_stocksViewModel, &StocksViewModel::errorOccurred, &m_dialogManager,
            &DialogManager::showErrorDialog);

    return m_stocksView;
}

UsersView *MainWindow::ensureUsersView()
{
    ensureUsersManagementState();

    if (m_usersView != nullptr) {
        return m_usersView;
    }

    m_usersView = new UsersView(*m_usersManagementViewModel, this);

    return m_usersView;
}

RolesView *MainWindow::ensureRolesView()
{
    ensureUsersManagementState();

    if (m_rolesView != nullptr) {
        return m_rolesView;
    }

    m_rolesView = new RolesView(*m_usersManagementViewModel, this);

    return m_rolesView;
}

ModuleStatusView *MainWindow::ensureModuleStatusView(MainMenuActions::Type actionType)
{
    const auto it = m_moduleStatusViews.find(actionType);
    if (it != m_moduleStatusViews.end()) {
        return it->second;
    }

    const auto spec = buildModuleStatusSpec(actionType);
    auto *view      = new ModuleStatusView(MainMenuActions::NAMES.at(actionType), spec.statusBadge,
                                           spec.summary, spec.currentState, spec.nextSteps, this);
    m_moduleStatusViews.emplace(actionType, view);

    return view;
}
