#pragma once

#include <string>

struct Producttype
{
    static constexpr const char* TABLE_NAME = "ProductType";
    
    std::string
        id,
        code,
        barcode,
        name,
        description,
        lastPrice,
        unit,
        isImported;
};
