#pragma once

#include <string>

#include "IEntity.hpp"

class User : public IEntity
{
public:
    std::string
        id,
        username,
        password,
        roleId;

    User(std::string id, std::string username, std::string password, std::string roleId)
        : id(std::move(id)), username(std::move(username)), password(std::move(password)), roleId(std::move(roleId)) {}

    Dataset &operator>>(Dataset &dataset) override
    {
        dataset[Keys::User::ID].push_back(id.empty() ? "" : id);
        dataset[Keys::User::USERNAME].push_back(username.empty() ? "" : username);
        dataset[Keys::User::PASSWORD].push_back(password.empty() ? "" : password);
        dataset[Keys::User::ROLE_ID].push_back(roleId.empty() ? "" : roleId);

        return dataset;
    }
};