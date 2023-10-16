#include "UiManager.hpp"

#include <QApplication>
#include <QQmlContext>

#include <string>

#include "Logging.hpp"
#include "QmlTypeRegistrar.hpp"

#include "Views\PurchaseView.hpp"

UiManager::UiManager(QObject *parent) noexcept
    : QObject(parent)
{
    SPDLOG_TRACE("UiManager::UiManager");

    QmlTypeRegistrar::registerTypes();

    connect(&m_engine, &QQmlApplicationEngine::objectCreated,
            this, [this](QObject *obj, const QUrl &objUrl)
            { if (!obj) SPDLOG_CRITICAL("Failed to load QML from URL: {}", objUrl.toString().toStdString()); });
}

UiManager::~UiManager()
{
    SPDLOG_TRACE("UiManager::~UiManager");
}

void UiManager::initUi()
{
    SPDLOG_TRACE("UiManager::init");

    initMainWindow();
    initTheme();
    initModules();
    initDialogues();
}

void UiManager::setTheme(Theme theme)
{
    const auto theme_str = theme == Theme::Dark ? "Dark" : "Light";
    SPDLOG_TRACE(std::string("UiManager::setTheme") + theme_str);

    if (m_theme != theme)
    {
        m_theme = theme;
        SPDLOG_INFO(std::string("Changing theme to ") + theme_str);
        emit themeChanged(theme);
    }
}

UiManager::Theme UiManager::theme() const
{
    SPDLOG_TRACE("UiManager::theme");
    return m_theme;
}

QFont UiManager::defaultFont() const
{
    // TODO: make possibility to setup
    QFont defaultFont("Helvetica");
    defaultFont.setPixelSize(18);
    return defaultFont;
}

void UiManager::initTheme()
{
    SPDLOG_TRACE("UiManager::initTheme");
    setTheme(Theme::Dark);
}

void UiManager::initMainWindow()
{
    SPDLOG_TRACE("UiManager::initMainWindow");

    if (m_engine.rootContext())
    {
        m_engine.rootContext()->setContextProperty("uiManager", this);
        m_engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
        if (m_engine.rootObjects().isEmpty())
            SPDLOG_CRITICAL("UiManager::initMainWindow Failed to load main window");
    }
    else
    {
        SPDLOG_CRITICAL("UiManager::initMainWindow m_engine is invalid");
    }
}

void UiManager::initModules()
{
    SPDLOG_TRACE("UiManager::initModules");

    // Purchase View
    m_purchaseView = new PurchaseView(&m_engine);
    m_purchaseView->render();
    connect(this, &UiManager::pushToStackRequested, this, &UiManager::showPurchaseOrdersView);
}

void UiManager::initDialogues()
{
    SPDLOG_TRACE("UiManager::initDialogues");
}

bool UiManager::addToStack(QQmlComponent *component)
{
    QObject *newViewInstance = component->create();
    if (!newViewInstance)
    {
        SPDLOG_ERROR("Failed to create instance from component");
        return false;
    }

    QQmlEngine::setObjectOwnership(newViewInstance, QQmlEngine::JavaScriptOwnership);

    QQuickItem *item = qobject_cast<QQuickItem *>(newViewInstance);
    if (!item)
    {
        SPDLOG_ERROR("Failed to cast created component instance to QQuickItem");
        return false;
    }

    emit pushToStackRequested(item);
    return true; // assuming the emit was successful. You can add more logic to confirm if needed.
}

void UiManager::showPurchaseOrdersView()
{
    if (!m_purchaseView)
    {
        SPDLOG_CRITICAL("m_purchaseView is nullptr. PurchaseView not initialized.");
        return;
    }

    if (addToStack(m_purchaseView->component()))
    {
        SPDLOG_INFO("PurchaseView rendered successfully");
    }
    else
    {
        SPDLOG_ERROR("Failed to add PurchaseView to TabView");
    }
}
