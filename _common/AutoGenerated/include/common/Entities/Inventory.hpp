#pragma once

#include <string>

namespace Common {
namespace Entities {

struct Inventory {
    static constexpr const char *TABLE_NAME             = "Inventory";
    static constexpr const char *ID_KEY                 = "id";
    static constexpr const char *PRODUCT_TYPE_ID_KEY    = "productTypeId";
    static constexpr const char *QUANTITY_AVAILABLE_KEY = "quantityAvailable";
    static constexpr const char *EMPLOYEE_ID_KEY        = "employeeId";

    std::string id{};
    std::string productTypeId{};
    std::string quantityAvailable{};
    std::string employeeId{};
};

} // namespace Entities
} // namespace Common
