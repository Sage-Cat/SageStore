#pragma once

#include <string>

struct Employee
{
    static constexpr const char* TABLE_NAME = "Employee";
    std::string
        id,
        name,
        number,
        email,
        address;
};
