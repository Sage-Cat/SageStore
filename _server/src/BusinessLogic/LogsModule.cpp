#include "LogsModule.hpp"

#include "Database/RepositoryManager.hpp"
#include "ModuleUtils.hpp"

#define _M "LogsModule"

namespace {
Common::Entities::Log buildLog(const Dataset &request, const std::string &resourceId)
{
    const auto userId =
        ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Log::USER_ID_KEY);
    if (!userId.empty()) {
        if (!ModuleUtils::isDigits(userId)) {
            throw ServerException(_M, "Invalid positive integer field: userId");
        }
        ModuleUtils::ensureIntegerFitsRange(userId, Common::Entities::Log::USER_ID_KEY, _M,
                                            "LogsModule::buildLog");
    }

    return Common::Entities::Log{
        .id = resourceId,
        .userId = userId,
        .action = ModuleUtils::getRequiredDatasetValue(request, Common::Entities::Log::ACTION_KEY,
                                                       _M, "LogsModule::buildLog"),
        .timestamp = ModuleUtils::getRequiredDatasetValue(
            request, Common::Entities::Log::TIMESTAMP_KEY, _M, "LogsModule::buildLog")};
}
} // namespace

LogsModule::LogsModule(RepositoryManager &repositoryManager)
{
    m_logsRepository = std::move(repositoryManager.getLogRepository());
}

LogsModule::~LogsModule() = default;

ResponseData LogsModule::executeTask(const RequestData &requestData)
{
    if (requestData.submodule != "entries") {
        throw ServerException(_M, "Unrecognized logs task");
    }

    if (requestData.method == "GET") {
        return getLogs();
    }
    if (requestData.method == "POST") {
        addLog(requestData.dataset);
        return {};
    }
    if (requestData.method == "PUT") {
        editLog(requestData.dataset, requestData.resourceId);
        return {};
    }
    if (requestData.method == "DELETE") {
        deleteLog(requestData.resourceId);
        return {};
    }

    throw ServerException(_M, "Unrecognized method");
}

ResponseData LogsModule::getLogs() const
{
    ResponseData response;
    response.dataset[Common::Entities::Log::ID_KEY]        = {};
    response.dataset[Common::Entities::Log::USER_ID_KEY]   = {};
    response.dataset[Common::Entities::Log::ACTION_KEY]    = {};
    response.dataset[Common::Entities::Log::TIMESTAMP_KEY] = {};

    for (const auto &entry : m_logsRepository->getAll()) {
        response.dataset[Common::Entities::Log::ID_KEY].emplace_back(entry.id);
        response.dataset[Common::Entities::Log::USER_ID_KEY].emplace_back(entry.userId);
        response.dataset[Common::Entities::Log::ACTION_KEY].emplace_back(entry.action);
        response.dataset[Common::Entities::Log::TIMESTAMP_KEY].emplace_back(entry.timestamp);
    }

    return response;
}

void LogsModule::addLog(const Dataset &request) const
{
    m_logsRepository->add(buildLog(request, ""));
}

void LogsModule::editLog(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Log");
    ModuleUtils::ensureEntityExists(m_logsRepository, Common::Entities::Log::ID_KEY, resourceId,
                                    _M, "Log entry does not exist");
    m_logsRepository->update(buildLog(request, resourceId));
}

void LogsModule::deleteLog(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Log");
    ModuleUtils::ensureEntityExists(m_logsRepository, Common::Entities::Log::ID_KEY, resourceId,
                                    _M, "Log entry does not exist");
    m_logsRepository->deleteResource(resourceId);
}
