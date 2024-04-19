#pragma once

#include <string>

struct Supplier
{
    static constexpr const char* TABLE_NAME = "Supplier";
    std::string
        id,
        name,
        number,
        email,
        address;
};
