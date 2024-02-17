#include "BusinessLogicFacade.hpp"

#include "SpdlogConfig.hpp"

std::string BusinessLogicFacade::executeTask(RequestData, BusinessLogicCallback)
{
    SPDLOG_TRACE("BusinessLogicFacade::executeTask");
    return std::string();
}
