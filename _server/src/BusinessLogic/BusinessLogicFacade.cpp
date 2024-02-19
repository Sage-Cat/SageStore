#include "BusinessLogicFacade.hpp"

#include "UsersModule.hpp"
#include "ServerException.hpp"
#include "Network/NetworkCommon.hpp"
#include "Database/RepositoryManager.hpp"

#include "DatasetCommon.hpp"
#include "SpdlogConfig.hpp"

BusinessLogicFacade::BusinessLogicFacade(RepositoryManager &repositoryManager)
    : m_usersModule(
          std::make_unique<UsersModule>(
              std::move(repositoryManager.getUsersRepository()),
              std::move(repositoryManager.getRolesRepository())))
{
    SPDLOG_TRACE("BusinessLogicFacade::BusinessLogicFacade");
}

BusinessLogicFacade::~BusinessLogicFacade()
{
    SPDLOG_TRACE("BusinessLogicFacade::~BusinessLogicFacade");
}

void BusinessLogicFacade::executeTask(RequestData requestData, BusinessLogicCallback callback)
{
    SPDLOG_TRACE("BusinessLogicFacade::executeTask");
    ResponseData responseData;
    try
    {
        if (requestData.module == "users")
        {
            responseData = m_usersModule->executeTask(requestData);
        }
    }
    catch (const ServerException &ex)
    {
        responseData.dataset[Keys::_ERROR] = {ex.componentName(), ex.what()};
    }
    catch (const std::exception &ex)
    {
        responseData.dataset[Keys::_ERROR] = {ex.what()};
    }
    catch (...)
    {
        SPDLOG_ERROR("BusinessLogicFacade::executeTask | Something bad happend");
        responseData.dataset[Keys::_ERROR] = {"Unreadable exception"};
    }

    // Send response to client
    callback(responseData);
}
