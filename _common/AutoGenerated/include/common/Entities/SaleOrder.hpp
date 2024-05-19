#pragma once

#include <string>

struct Saleorder {
    static constexpr const char *TABLE_NAME = "SaleOrder";

    std::string id, date, userId, contactId, employeeId, status;
};
