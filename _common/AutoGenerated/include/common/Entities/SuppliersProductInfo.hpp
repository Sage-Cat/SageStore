#pragma once

#include <string>

namespace Common {
namespace Entities {

struct SuppliersProductInfo {
    static constexpr const char *TABLE_NAME          = "SuppliersProductInfo";
    static constexpr const char *ID_KEY              = "id";
    static constexpr const char *SUPPLIER_ID_KEY     = "supplierID";
    static constexpr const char *PRODUCT_TYPE_ID_KEY = "productTypeId";
    static constexpr const char *CODE_KEY            = "code";

    std::string id{};
    std::string supplierID{};
    std::string productTypeId{};
    std::string code{};
};

} // namespace Entities
} // namespace Common
