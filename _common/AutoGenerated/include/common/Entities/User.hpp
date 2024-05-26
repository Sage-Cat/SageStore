#pragma once

#include <string>

namespace Common {
namespace Entities {

struct User {
    static constexpr const char *TABLE_NAME   = "User";
    static constexpr const char *ID_KEY       = "id";
    static constexpr const char *USERNAME_KEY = "username";
    static constexpr const char *PASSWORD_KEY = "password";
    static constexpr const char *ROLE_ID_KEY  = "roleId";

    std::string id{};
    std::string username{};
    std::string password{};
    std::string roleId{};
};

} // namespace Entities
} // namespace Common
