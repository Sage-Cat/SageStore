#include "SqliteDatabaseManager.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>
#include <string_view>

#include <sqlite3.h>

#include "ServerException.hpp"
#include "common/SpdlogConfig.hpp"

#define _M "SqliteDatabaseManager"

class SqliteQueryResult : public IQueryResult
{
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt;

public:
    SqliteQueryResult(sqlite3_stmt *stmtPtr)
        : stmt(stmtPtr, sqlite3_finalize) {}

    bool next() override
    {
        int stepResult = sqlite3_step(stmt.get());
        if (stepResult == SQLITE_ROW)
        {
            return true;
        }
        else if (stepResult == SQLITE_DONE)
        {
            return false;
        }
        else
        {
            const std::string errorMsg = "SQL error: " + std::string(sqlite3_errmsg(sqlite3_db_handle(stmt.get())));
            SPDLOG_ERROR(errorMsg);
            throw ServerException(_M, errorMsg);
        }
    }

    std::string getString(int columnIndex) const override
    {
        auto text = sqlite3_column_text(stmt.get(), columnIndex);
        return text ? reinterpret_cast<const char *>(text) : "";
    }
};

class SqliteDatabaseManager::Impl
{
public:
    std::string m_dbPath, m_createDatabaseScriptPath;
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_db{nullptr, sqlite3_close};

    Impl(std::string_view path, std::string_view createDatabaseScriptPath)
        : m_dbPath(path),
          m_createDatabaseScriptPath(createDatabaseScriptPath) {}

    bool open()
    {
        SPDLOG_TRACE("SQLiteDatabaseManager::open");
        const auto fileExists = std::filesystem::exists(m_dbPath); // Use C++17 std::filesystem for file existence check

        sqlite3 *dbRaw = nullptr;
        if (sqlite3_open(m_dbPath.c_str(), &dbRaw) != SQLITE_OK)
        {
            SPDLOG_ERROR("Failed to open database: {}", sqlite3_errmsg(dbRaw));
            if (dbRaw)
                sqlite3_close(dbRaw);
            return false;
        }

        m_db.reset(dbRaw);

        if (!fileExists && !initializeDatabaseSchema())
        {
            SPDLOG_ERROR("Failed to initialize database schema.");
            return false;
        }

        SPDLOG_INFO("Database opened successfully");
        return true;
    }

    void close()
    {
        m_db.reset();
        SPDLOG_INFO("Database closed successfully");
    }

    std::shared_ptr<IQueryResult> executeQuery(std::string_view query, const std::vector<std::string> &params)
    {
        SPDLOG_TRACE("SQLiteDatabaseManager::executeQuery");
        SPDLOG_DEBUG("SQLiteDatabaseManager::executeQuery | query = \"{}\"", query);

        sqlite3_stmt *stmtRaw = nullptr;
        if (sqlite3_prepare_v2(m_db.get(), query.data(), -1, &stmtRaw, nullptr) != SQLITE_OK)
        {
            SPDLOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(m_db.get()));
            throw ServerException(_M, "Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db.get())));
        }

        if (!stmtRaw)
        {
            SPDLOG_ERROR("Failed to create the statement, stmtRaw is null.");
            throw ServerException(_M, "Failed to create the statement, stmtRaw is null.");
        }

        // Binding parameters
        for (size_t i = 0; i < params.size(); ++i)
        {
            if (sqlite3_bind_text(stmtRaw, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
            {
                SPDLOG_ERROR("Failed to bind parameter at position {}: {}", i + 1, sqlite3_errmsg(m_db.get()));
                sqlite3_finalize(stmtRaw); // Clean up before throwing
                throw ServerException(_M, "Failed to bind parameter at position " + std::to_string(i + 1) + ": " + std::string(sqlite3_errmsg(m_db.get())));
            }
        }

        auto queryResult = std::make_shared<SqliteQueryResult>(stmtRaw);

        // For non-select query we just make step and return nullptr
        if (!isSelectQuery(query))
        {
            queryResult->next();
            queryResult.reset(); // nullptr
        }

        return queryResult;
    }

    bool initializeDatabaseSchema()
    {
        SPDLOG_TRACE("SQLiteDatabaseManager::initializeDatabaseSchema");
        SPDLOG_DEBUG("Initializing database schema | using sql script: {}", m_createDatabaseScriptPath);

        if (!std::filesystem::exists(m_createDatabaseScriptPath))
        {
            SPDLOG_ERROR("Could not open SQL script file: {}", m_createDatabaseScriptPath);
            return false;
        }

        std::ifstream sqlFile(m_createDatabaseScriptPath);
        std::string sqlCommands((std::istreambuf_iterator<char>(sqlFile)),
                                std::istreambuf_iterator<char>());

        char *errMsg = nullptr;
        if (sqlite3_exec(m_db.get(), sqlCommands.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            SPDLOG_ERROR("Initialize database schema error: {}", errMsg);
            sqlite3_free(errMsg);
            return false;
        }

        SPDLOG_INFO("Database schema initialized successfully");
        return true;
    }

    bool isSelectQuery(std::string_view query)
    {
        return query.find("SELECT") == 0 || query.find("select") == 0;
    }
};

SqliteDatabaseManager::SqliteDatabaseManager(std::string_view dbPath, std::string_view createDatabaseScriptPath)
    : pImpl(std::make_unique<Impl>(dbPath, createDatabaseScriptPath)) {}

SqliteDatabaseManager::~SqliteDatabaseManager() = default;

bool SqliteDatabaseManager::open()
{
    return pImpl->open();
}

void SqliteDatabaseManager::close()
{
    pImpl->close();
}

std::shared_ptr<IQueryResult> SqliteDatabaseManager::executeQuery(std::string_view query, const std::vector<std::string> &params)
{
    return pImpl->executeQuery(query, params);
}
