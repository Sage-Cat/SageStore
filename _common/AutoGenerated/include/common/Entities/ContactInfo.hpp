#pragma once

#include <string>

namespace Common {
namespace Entities {

struct ContactInfo {
    static constexpr const char *TABLE_NAME     = "ContactInfo";
    static constexpr const char *ID_KEY         = "id";
    static constexpr const char *CONTACT_ID_KEY = "contactID";
    static constexpr const char *NAME_KEY       = "name";
    static constexpr const char *VALUE_KEY      = "value";

    std::string id{};
    std::string contactID{};
    std::string name{};
    std::string value{};
};

} // namespace Entities
} // namespace Common
