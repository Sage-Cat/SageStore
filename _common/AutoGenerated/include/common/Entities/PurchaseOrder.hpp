#pragma once

#include <string>

struct Purchaseorder
{
    static constexpr const char* TABLE_NAME = "PurchaseOrder";
    
    std::string
        id,
        date,
        userId,
        supplierId,
        status;
};
