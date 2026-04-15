#include "SystemModule.hpp"

#include "ServerException.hpp"

#define _M "SystemModule"

ResponseData SystemModule::executeTask(const RequestData &requestData)
{
    if (requestData.submodule != "health") {
        throw ServerException(_M, "Unrecognized system task");
    }

    if (requestData.method != "GET") {
        throw ServerException(_M, "Health endpoint is read-only");
    }

    ResponseData response;
    response.dataset["status"]  = {"ok"};
    response.dataset["service"] = {"SageStoreServer"};
    response.dataset["ready"]   = {"true"};
    return response;
}
