#pragma once

#include <string>

struct Inventory {
    static constexpr const char *TABLE_NAME = "Inventory";

    std::string id, productTypeId, quantityAvailable, employeeId;
};
