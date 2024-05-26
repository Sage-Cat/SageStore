#pragma once

#include <string>

namespace Common {
namespace Entities {

struct Log {
    static constexpr const char *TABLE_NAME    = "Log";
    static constexpr const char *ID_KEY        = "id";
    static constexpr const char *USER_ID_KEY   = "userId";
    static constexpr const char *ACTION_KEY    = "action";
    static constexpr const char *TIMESTAMP_KEY = "timestamp";

    std::string id{};
    std::string userId{};
    std::string action{};
    std::string timestamp{};
};

} // namespace Entities
} // namespace Common
