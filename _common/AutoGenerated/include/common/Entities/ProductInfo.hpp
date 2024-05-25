#pragma once

#include <string>

namespace Common {
namespace Entities {

struct ProductInfo {
    static constexpr const char *TABLE_NAME          = "ProductInfo";
    static constexpr const char *ID_KEY              = "id";
    static constexpr const char *PRODUCT_TYPE_ID_KEY = "productTypeId";
    static constexpr const char *NAME_KEY            = "name";
    static constexpr const char *VALUE_KEY           = "value";

    std::string id{};
    std::string productTypeId{};
    std::string name{};
    std::string value{};
};

} // namespace Entities
} // namespace Common
