#pragma once

#include <string>

namespace Common {
namespace Entities {

struct Supplier {
    static constexpr const char *TABLE_NAME  = "Supplier";
    static constexpr const char *ID_KEY      = "id";
    static constexpr const char *NAME_KEY    = "name";
    static constexpr const char *NUMBER_KEY  = "number";
    static constexpr const char *EMAIL_KEY   = "email";
    static constexpr const char *ADDRESS_KEY = "address";

    std::string id{};
    std::string name{};
    std::string number{};
    std::string email{};
    std::string address{};
};

} // namespace Entities
} // namespace Common
