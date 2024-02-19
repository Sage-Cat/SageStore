#pragma once

#include <string>
#include <string_view>
#include <memory>

#include <sqlite3.h>

class DatabaseManager
{
public:
    explicit DatabaseManager(std::string_view dbPath);
    ~DatabaseManager() = default;

    bool open();
    void close();
    bool executeSQL(std::string_view query);

    void prepareStatement(std::string_view query);
    void bind(int index, std::string_view value);
    bool step();
    std::string columnText(int index);
    int columnInt(int index);
    void finalizeStatement();

private:
    void checkSQLiteResult(int resultCode, std::string_view context);

private:
    std::string m_dbPath;
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_db{nullptr, sqlite3_close};
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> m_stmt{nullptr, sqlite3_finalize};
};
