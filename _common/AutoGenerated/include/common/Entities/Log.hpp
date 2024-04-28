#pragma once

#include <string>

struct Log
{
    static constexpr const char* TABLE_NAME = "Log";
    
    std::string
        id,
        userId,
        action,
        timestamp;
};
