#pragma once

#include <string>
#include <string_view>

#include "IDatabaseManager.hpp"

class SqliteDatabaseManager : public IDatabaseManager
{
public:
    explicit SqliteDatabaseManager(std::string_view dbPath, std::string_view createDatabaseScriptPath);
    ~SqliteDatabaseManager() override;

    bool open() override;
    void close() override;

    std::shared_ptr<IQueryResult> executeQuery(std::string_view query, const std::vector<std::string> &params) override;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
