#include "BusinessLogic.hpp"

#include <utility>

#include "AnalyticsModule.hpp"
#include "Database/RepositoryManager.hpp"
#include "InventoryModule.hpp"
#include "IBusinessModule.hpp"
#include "LogsModule.hpp"
#include "ManagementModule.hpp"
#include "ModuleUtils.hpp"
#include "Network/NetworkGeneral.hpp"
#include "PurchaseModule.hpp"
#include "ServerException.hpp"
#include "SalesModule.hpp"
#include "UsersModule.hpp"

#include "common/Entities/Log.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

#define _M "BusinessLogic"

namespace {
std::string getOptionalDatasetValue(const Dataset &request, const std::string &key)
{
    const auto dataIt = request.find(key);
    if (dataIt == request.end() || dataIt->second.empty()) {
        return {};
    }

    return dataIt->second.front();
}

std::string buildAuditAction(const RequestData &requestData)
{
    std::string action = requestData.module + "/" + requestData.submodule + " " + requestData.method;
    if (!requestData.resourceId.empty()) {
        action += " #" + requestData.resourceId;
    }
    return action;
}
} // namespace

BusinessLogic::BusinessLogic(RepositoryManager &repositoryManager)
{
    m_logRepository = repositoryManager.getLogRepository();
    m_modules.emplace("users", std::make_unique<UsersModule>(repositoryManager));
    m_modules.emplace("inventory", std::make_unique<InventoryModule>(repositoryManager));
    m_modules.emplace("management", std::make_unique<ManagementModule>(repositoryManager));
    m_modules.emplace("purchase", std::make_unique<PurchaseModule>(repositoryManager));
    m_modules.emplace("sales", std::make_unique<SalesModule>(repositoryManager));
    m_modules.emplace("analytics", std::make_unique<AnalyticsModule>(repositoryManager));
    m_modules.emplace("logs", std::make_unique<LogsModule>(repositoryManager));

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
        if (m_logRepository != nullptr && requestData.method != "GET" &&
            requestData.module != "logs") {
            try {
                m_logRepository->add(Common::Entities::Log{
                    .id = "",
                    .userId = getOptionalDatasetValue(requestData.dataset, "userId"),
                    .action = buildAuditAction(requestData),
                    .timestamp = ModuleUtils::currentTimestamp()});
            } catch (const std::exception &ex) {
                SPDLOG_WARN("BusinessLogic::executeTask | Failed to write audit log: {}", ex.what());
            }
        }
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
