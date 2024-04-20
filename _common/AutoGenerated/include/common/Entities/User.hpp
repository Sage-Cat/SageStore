#pragma once

#include <string>

struct User
{
    static constexpr const char* TABLE_NAME = "User";
    
    std::string
        id,
        username,
        password,
        roleId;
};
