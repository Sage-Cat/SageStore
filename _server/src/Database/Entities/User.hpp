#pragma once

#include <string>

class User
{
public:
    std::string
        id,
        username,
        password,
        roleId;

    User(std::string id, std::string username, std::string password, std::string roleId)
        : id(std::move(id)), username(std::move(username)), password(std::move(password)), roleId(std::move(roleId)) {}

    Dataset &operator>>(Dataset &dataset)
    {
        dataset["id"].push_back(id.empty() ? "" : id);
        dataset["username"].push_back(username.empty() ? "" : username);
        dataset["password"].push_back(password.empty() ? "" : password);
        dataset["roleId"].push_back(roleId.empty() ? "" : roleId);

        return dataset;
    }
};