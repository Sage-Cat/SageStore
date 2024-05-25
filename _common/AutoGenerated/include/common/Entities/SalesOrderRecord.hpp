#pragma once

#include <string>

namespace Common {
namespace Entities {

struct SalesOrderRecord {
    static constexpr const char *TABLE_NAME          = "SalesOrderRecord";
    static constexpr const char *ID_KEY              = "id";
    static constexpr const char *ORDER_ID_KEY        = "orderId";
    static constexpr const char *PRODUCT_TYPE_ID_KEY = "productTypeId";
    static constexpr const char *QUANTITY_KEY        = "quantity";
    static constexpr const char *PRICE_KEY           = "price";

    std::string id{};
    std::string orderId{};
    std::string productTypeId{};
    std::string quantity{};
    std::string price{};
};

} // namespace Entities
} // namespace Common
