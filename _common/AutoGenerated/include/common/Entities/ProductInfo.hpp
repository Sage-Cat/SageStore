#pragma once

#include <string>

struct Productinfo {
    static constexpr const char *TABLE_NAME = "ProductInfo";

    std::string id, productTypeId, name, value;
};
