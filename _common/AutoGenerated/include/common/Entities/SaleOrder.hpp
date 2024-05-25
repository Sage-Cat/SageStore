#pragma once

#include <string>

namespace Common {
namespace Entities {

struct SaleOrder {
    static constexpr const char *TABLE_NAME      = "SaleOrder";
    static constexpr const char *ID_KEY          = "id";
    static constexpr const char *DATE_KEY        = "date";
    static constexpr const char *USER_ID_KEY     = "userId";
    static constexpr const char *CONTACT_ID_KEY  = "contactId";
    static constexpr const char *EMPLOYEE_ID_KEY = "employeeId";
    static constexpr const char *STATUS_KEY      = "status";

    std::string id{};
    std::string date{};
    std::string userId{};
    std::string contactId{};
    std::string employeeId{};
    std::string status{};
};

} // namespace Entities
} // namespace Common
