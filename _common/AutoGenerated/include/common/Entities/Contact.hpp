#pragma once

#include <string>

struct Contact
{
    static constexpr const char* TABLE_NAME = "Contact";
    
    std::string
        id,
        name,
        type,
        email,
        phone;
};
