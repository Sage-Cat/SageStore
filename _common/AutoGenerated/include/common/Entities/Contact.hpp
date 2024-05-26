#pragma once

#include <string>

namespace Common {
namespace Entities {

struct Contact {
    static constexpr const char *TABLE_NAME = "Contact";
    static constexpr const char *ID_KEY     = "id";
    static constexpr const char *NAME_KEY   = "name";
    static constexpr const char *TYPE_KEY   = "type";
    static constexpr const char *EMAIL_KEY  = "email";
    static constexpr const char *PHONE_KEY  = "phone";

    std::string id{};
    std::string name{};
    std::string type{};
    std::string email{};
    std::string phone{};
};

} // namespace Entities
} // namespace Common
