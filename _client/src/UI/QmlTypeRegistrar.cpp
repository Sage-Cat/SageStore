#include "QmlTypeRegistrar.hpp"
#include "UiManager.hpp"

#include "Logging.hpp"

namespace QmlTypeRegistrar
{
    void registerTypes() noexcept
    {
        SPDLOG_INFO("QmlTypeRegistrar::registerTypes");
        qmlRegisterType<UiManager>("com.sage", 1, 0, "UiManager");
        qmlRegisterUncreatableMetaObject(UiManager::staticMetaObject, "com.sage", 1, 0, "Theme", "Error: only enums");
    }
}