#include "ServerStartupConfig.hpp"

#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <string_view>

#include "common/SpdlogConfig.hpp"

namespace ServerStartupConfig {

std::string envOrDefault(const char *name, const char *fallback)
{
    const char *value = std::getenv(name);
    return value != nullptr && *value != '\0' ? value : fallback;
}

unsigned short envOrDefaultPort(const char *name, unsigned short fallback)
{
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return fallback;
    }

    try {
        std::size_t parsedLength = 0;
        const long parsedValue   = std::stol(value, &parsedLength);
        if (parsedLength != std::string_view(value).size() || parsedValue < 0 ||
            parsedValue > static_cast<long>(std::numeric_limits<unsigned short>::max())) {
            throw std::out_of_range("port out of range");
        }

        return static_cast<unsigned short>(parsedValue);
    } catch (...) {
        SPDLOG_WARN("Invalid port in {}='{}', using default {}", name, value, fallback);
        return fallback;
    }
}

} // namespace ServerStartupConfig
