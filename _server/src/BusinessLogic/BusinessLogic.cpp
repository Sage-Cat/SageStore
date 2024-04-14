#include "BusinessLogic/BusinessLogic.hpp"

#include "BusinessLogic/UsersModule.hpp"
#include "ServerException.hpp"
#include "Network/NetworkCommon.hpp"

#include "DataCommon.hpp"
#include "SpdlogConfig.hpp"

BusinessLogic::BusinessLogic(RepositoryManager &repositoryManager)
    : m_usersModule(std::make_unique<UsersModule>(repositoryManager))
{
    SPDLOG_TRACE("BusinessLogic::BusinessLogic");
}

BusinessLogic::~BusinessLogic()
{
    SPDLOG_TRACE("BusinessLogic::~BusinessLogic");
}

void BusinessLogic::executeTask(RequestData requestData, BusinessLogicCallback callback)
{
    SPDLOG_TRACE("BusinessLogic::executeTask");
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
        SPDLOG_ERROR("BusinessLogic::executeTask | Something bad happend");
        responseData.dataset[Keys::_ERROR] = {"Unreadable exception"};
    }

    // Send response to client
    callback(responseData);
}
