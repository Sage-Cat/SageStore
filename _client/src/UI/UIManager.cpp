#include "UI/UIManager.hpp"

#include <utility>

UIManager::UIManager(QObject *parent)
    : QObject(parent),
      m_engine(std::make_unique<QQmlApplicationEngine>())
{
    m_engine->load(QUrl(QStringLiteral("qrc:/mainwindow.qml")));

    if (m_engine->rootObjects().isEmpty())
    {
        // TODO: error handling, consider emitting errorOccurred signal
        emit errorOccurred("Failed to load main window");
    }
}

UIManager::~UIManager() = default;

void UIManager::init()
{
    initModules();
    initDialogues();
}

void UIManager::onModuleEvent(int eventCode)
{
    // TODO: Handle module events
    emit uiEventOccurred(eventCode);
}

void UIManager::initModules()
{
    // TODO: Initialize UI modules
}

void UIManager::initDialogues()
{
    // TODO: Initialize UI dialogues or windows
}
