#pragma once

#include <string>

namespace Common {
namespace Entities {

struct Role {
    static constexpr const char *TABLE_NAME = "Role";
    static constexpr const char *ID_KEY     = "id";
    static constexpr const char *NAME_KEY   = "name";

    std::string id{};
    std::string name{};
};

} // namespace Entities
} // namespace Common
