#pragma once

#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

#include "DataSpecs.hpp"
#include "Database/IRepository.hpp"
#include "ServerException.hpp"

#include "common/SpdlogConfig.hpp"

namespace ModuleUtils {
inline std::string getOptionalDatasetValue(const Dataset &request, const std::string &key)
{
    const auto dataIt = request.find(key);
    if (dataIt == request.end() || dataIt->second.empty()) {
        return {};
    }

    return dataIt->second.front();
}

inline std::string getRequiredDatasetValue(const Dataset &request, const std::string &key,
                                           const char *component, const char *context)
{
    const auto value = getOptionalDatasetValue(request, key);
    if (value.empty()) {
        SPDLOG_ERROR("{} | Missing required dataset key: {}", context, key);
        throw ServerException(component, "Missing required field: " + key);
    }

    return value;
}

inline bool isDigits(const std::string &value)
{
    return !value.empty() &&
           std::all_of(value.begin(), value.end(), [](unsigned char c) { return std::isdigit(c); });
}

inline void ensureIntegerFitsRange(const std::string &value, const std::string &key,
                                   const char *component, const char *context)
{
    try {
        const auto parsed = std::stoull(value);
        if (parsed > static_cast<unsigned long long>(std::numeric_limits<long long>::max())) {
            SPDLOG_ERROR("{} | Integer is out of range for dataset key: {}", context, key);
            throw ServerException(component, "Integer field out of range: " + key);
        }
    } catch (const ServerException &) {
        throw;
    } catch (...) {
        SPDLOG_ERROR("{} | Integer is out of range for dataset key: {}", context, key);
        throw ServerException(component, "Integer field out of range: " + key);
    }
}

inline std::string getRequiredNonNegativeInteger(const Dataset &request, const std::string &key,
                                                 const char *component, const char *context)
{
    const auto value = getRequiredDatasetValue(request, key, component, context);
    if (!isDigits(value)) {
        SPDLOG_ERROR("{} | Invalid non-negative integer for dataset key: {}", context, key);
        throw ServerException(component, "Invalid integer field: " + key);
    }

    ensureIntegerFitsRange(value, key, component, context);
    return value;
}

inline std::string getRequiredPositiveInteger(const Dataset &request, const std::string &key,
                                              const char *component, const char *context)
{
    const auto value = getRequiredNonNegativeInteger(request, key, component, context);
    if (value == "0") {
        SPDLOG_ERROR("{} | Integer must be positive for dataset key: {}", context, key);
        throw ServerException(component, "Invalid positive integer field: " + key);
    }

    return value;
}

inline std::string getRequiredNumber(const Dataset &request, const std::string &key,
                                     const char *component, const char *context)
{
    const auto value = getRequiredDatasetValue(request, key, component, context);
    try {
        (void)std::stod(value);
    } catch (...) {
        SPDLOG_ERROR("{} | Invalid numeric field: {}", context, key);
        throw ServerException(component, "Invalid numeric field: " + key);
    }

    return value;
}

inline std::string currentTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tmValue{};
#if defined(_WIN32)
    gmtime_s(&tmValue, &time);
#else
    gmtime_r(&time, &tmValue);
#endif

    std::ostringstream stream;
    stream << std::put_time(&tmValue, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

inline void ensureResourceId(const std::string &resourceId, const char *component,
                             const char *resourceName)
{
    if (resourceId.empty()) {
        throw ServerException(component, std::string(resourceName) + " ID is empty");
    }
}

template <typename T>
inline void ensureEntityExists(const std::shared_ptr<IRepository<T>> &repository,
                               const std::string &fieldName, const std::string &value,
                               const char *component, const std::string &message)
{
    if (repository->getByField(fieldName, value).empty()) {
        throw ServerException(component, message);
    }
}
} // namespace ModuleUtils
