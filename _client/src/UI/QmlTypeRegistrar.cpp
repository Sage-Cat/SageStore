#include "QmlTypeRegistrar.hpp"
#include "UIManager.hpp"

#include "logging.hpp"

namespace QmlTypeRegistrar
{
    void registerTypes() noexcept
    {
        SPDLOG_INFO("QmlTypeRegistrar::registerTypes");
        qmlRegisterType<UIManager>("com.sage", 1, 0, "UIManager");
        qmlRegisterUncreatableMetaObject(UIManager::staticMetaObject, "com.sage", 1, 0, "Theme", "Error: only enums");
    }
}