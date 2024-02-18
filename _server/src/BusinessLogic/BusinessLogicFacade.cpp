#include "BusinessLogicFacade.hpp"

#include "SpdlogConfig.hpp"

void BusinessLogicFacade::executeTask(RequestData, BusinessLogicCallback callback)
{
    SPDLOG_TRACE("BusinessLogicFacade::executeTask");
    callback(ResponseData());
}
