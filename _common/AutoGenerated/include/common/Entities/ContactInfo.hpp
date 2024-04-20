#pragma once

#include <string>

struct Contactinfo
{
    static constexpr const char* TABLE_NAME = "ContactInfo";
    
    std::string
        id,
        contactID,
        name,
        value;
};
