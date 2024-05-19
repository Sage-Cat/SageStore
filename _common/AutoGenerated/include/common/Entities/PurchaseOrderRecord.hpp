#pragma once

#include <string>

struct Purchaseorderrecord {
    static constexpr const char *TABLE_NAME = "PurchaseOrderRecord";

    std::string id, orderId, productTypeId, quantity, price;
};
