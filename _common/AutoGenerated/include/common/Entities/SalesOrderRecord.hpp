#pragma once

#include <string>

struct Salesorderrecord
{
    static constexpr const char* TABLE_NAME = "SalesOrderRecord";
    
    std::string
        id,
        orderId,
        productTypeId,
        quantity,
        price;
};
