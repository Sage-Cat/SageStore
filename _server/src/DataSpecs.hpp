#pragma once

#include <string>
#include <list>
#include <unordered_map>
#include <functional>

struct RequestData;
struct ResponseData;

using Data = std::list<std::string>;
using Dataset = std::unordered_map<std::string, Data>;
using BusinessLogicCallback = std::function<void(const ResponseData &)>;

/**
 * @struct RequestData
 * @brief Structure to hold information about an HTTP request.
 */
struct RequestData
{
    std::string module;    ///< Main module, that will proceed request, like 'users' or 'sales'.
    std::string submodule; ///< Module spesific process, like 'login' for users, 'records' for sales.
    std::string method;    ///< The HTTP method of the request, such as 'GET' or 'POST'.
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