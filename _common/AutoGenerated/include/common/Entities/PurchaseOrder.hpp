#pragma once

#include <string>

namespace Common {
namespace Entities {

struct PurchaseOrder {
    static constexpr const char *TABLE_NAME      = "PurchaseOrder";
    static constexpr const char *ID_KEY          = "id";
    static constexpr const char *DATE_KEY        = "date";
    static constexpr const char *USER_ID_KEY     = "userId";
    static constexpr const char *SUPPLIER_ID_KEY = "supplierId";
    static constexpr const char *STATUS_KEY      = "status";

    std::string id{};
    std::string date{};
    std::string userId{};
    std::string supplierId{};
    std::string status{};
};

} // namespace Entities
} // namespace Common
