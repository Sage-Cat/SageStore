#pragma once

#include <string>

struct Suppliersproductinfo
{
    static constexpr const char* TABLE_NAME = "SuppliersProductInfo";
    
    std::string
        id,
        supplierID,
        productTypeId,
        code;
};
