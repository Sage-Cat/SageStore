#pragma once

#include <functional>

#include "common/DataTypes.hpp"

struct RequestData;
struct ResponseData;

using BusinessLogicCallback = std::function<void(ResponseData)>;

/**
 * @struct RequestData
 * @brief Structure to hold information about an HTTP request.
 */
struct RequestData
{
    std::string module;    ///< Main module, that will proceed request, like 'users' or 'sales'.
    std::string submodule; ///< Module spesific process, like 'login' for users, 'records' for sales.
    std::string method;    ///< The HTTP method of the request, such as 'GET' or 'POST'.
    std::string resourceId; ///< ID of specific resource for methods like PUT or DELETE
    Dataset dataset;
};

/**
 * @struct ResponseData
 * @brief Structure to hold information about an response, that will be sent to client.
 */
struct ResponseData
{
    Dataset dataset;
};