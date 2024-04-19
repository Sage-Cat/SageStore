#pragma once

#include <string>

struct Role
{
    static constexpr const char* TABLE_NAME = "Role";
    std::string
        id,
        name;
};
