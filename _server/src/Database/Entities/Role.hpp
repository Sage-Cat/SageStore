#pragma once

#include <string>

#include "IEntity.hpp"

class Role : public IEntity
{
public:
    std::string
        id,
        name;

    Role(std::string id, std::string name)
        : id(std::move(id)), name(std::move(name)) {}
    explicit Role(std::string name) : name(std::move(name))
    {
    }

    Dataset &operator>>(Dataset &dataset) override
    {
        dataset[Keys::Role::ID].push_back(id.empty() ? "" : id);
        dataset[Keys::Role::NAME].push_back(name.empty() ? "" : name);

        return dataset;
    }
};