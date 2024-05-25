#pragma once

#include <string>

namespace Common {
namespace Entities {

struct ProductType {
    static constexpr const char *TABLE_NAME      = "ProductType";
    static constexpr const char *ID_KEY          = "id";
    static constexpr const char *CODE_KEY        = "code";
    static constexpr const char *BARCODE_KEY     = "barcode";
    static constexpr const char *NAME_KEY        = "name";
    static constexpr const char *DESCRIPTION_KEY = "description";
    static constexpr const char *LAST_PRICE_KEY  = "lastPrice";
    static constexpr const char *UNIT_KEY        = "unit";
    static constexpr const char *IS_IMPORTED_KEY = "isImported";

    std::string id{};
    std::string code{};
    std::string barcode{};
    std::string name{};
    std::string description{};
    std::string lastPrice{};
    std::string unit{};
    std::string isImported{};
};

} // namespace Entities
} // namespace Common
