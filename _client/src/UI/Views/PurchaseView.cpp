#include "PurchaseView.hpp"

#include <QUrl>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include "Logging.hpp"

PurchaseView::PurchaseView(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent), m_engine(engine)
{
    m_component = new QQmlComponent(m_engine, QUrl(QStringLiteral("qrc:/PurchaseOrdersView.qml")), this);
}

PurchaseView::~PurchaseView()
{
    delete m_purchaseViewInstance;
    // m_component has 'this' as its parent and will be automatically deleted when this QObject-derived class is deleted
}

void PurchaseView::render()
{
    if (m_component->isError())
    {
        for (const auto &error : m_component->errors())
            SPDLOG_CRITICAL("QML Error: {}", error.toString().toStdString());
    }

    if (!m_component->isReady())
    {
        SPDLOG_CRITICAL("PurchaseView::render - Component not ready");
        return;
    }

    if (!m_purchaseViewInstance)
    {
        m_purchaseViewInstance = m_component->create();
        if (!m_purchaseViewInstance)
        {
            SPDLOG_CRITICAL("Failed to create the purchaseView instance");
            return;
        }
    }

    SPDLOG_INFO("PurchaseView rendered successfully");
}

QQmlComponent *PurchaseView::component() const
{
    return m_component;
}
