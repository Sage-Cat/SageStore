#pragma once

#include <string>

class Role
{
public:
    std::string
        id,
        name;

    Role(std::string id, std::string name)
        : id(std::move(id)), name(std::move(name)) {}
};