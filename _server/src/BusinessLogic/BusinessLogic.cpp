#include "BusinessLogic.hpp"

#include <array>
#include <utility>

#include "IBusinessModule.hpp"
#include "Network/NetworkGeneral.hpp"
#include "ServerException.hpp"
#include "UsersModule.hpp"

#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

#define _M "BusinessLogic"

namespace {
class NotImplementedModule : public IBusinessModule {
public:
    explicit NotImplementedModule(std::string moduleName, std::string componentName)
        : m_moduleName(std::move(moduleName)), m_componentName(std::move(componentName))
    {
    }

    ResponseData executeTask(const RequestData &requestData) override
    {
        (void)requestData;
        throw ServerException(m_componentName,
                              "Module `" + m_moduleName + "` is planned but not implemented yet");
    }

private:
    std::string m_moduleName;
    std::string m_componentName;
};
} // namespace

BusinessLogic::BusinessLogic(RepositoryManager &repositoryManager)
{
    m_modules.emplace("users", std::make_unique<UsersModule>(repositoryManager));

    constexpr std::array<std::pair<const char *, const char *>, 6> PLANNED_MODULES{{
        {"purchase", "PurchaseModule"},
        {"sales", "SalesModule"},
        {"inventory", "InventoryModule"},
        {"management", "ManagementModule"},
        {"analytics", "AnalyticsModule"},
        {"logs", "LogsModule"},
    }};

    for (const auto &[moduleName, componentName] : PLANNED_MODULES) {
        m_modules.emplace(moduleName,
                          std::make_unique<NotImplementedModule>(moduleName, componentName));
    }

    SPDLOG_TRACE("BusinessLogic::BusinessLogic");
}

BusinessLogic::~BusinessLogic() { SPDLOG_TRACE("BusinessLogic::~BusinessLogic"); }

void BusinessLogic::executeTask(RequestData requestData, BusinessLogicCallback callback)
{
    SPDLOG_TRACE("BusinessLogic::executeTask");
    ResponseData responseData;
    try {
        if (requestData.module.empty()) {
            throw ServerException(_M, "Request module is empty");
        }

        const auto moduleIt = m_modules.find(requestData.module);
        if (moduleIt == m_modules.end()) {
            throw ServerException(_M, "Unrecognized module: " + requestData.module);
        }

        responseData = moduleIt->second->executeTask(requestData);
    } catch (const ServerException &ex) {
        responseData.dataset[Keys::_ERROR] = {ex.componentName(), ex.what()};
    } catch (const std::exception &ex) {
        responseData.dataset[Keys::_ERROR] = {ex.what()};
    } catch (...) {
        SPDLOG_ERROR("BusinessLogic::executeTask | Something bad happend");
        responseData.dataset[Keys::_ERROR] = {"Unreadable exception"};
    }

    // Send response to client
    callback(responseData);
}
