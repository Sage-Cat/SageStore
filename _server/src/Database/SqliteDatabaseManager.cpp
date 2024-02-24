#include "SqliteDatabaseManager.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>
#include <string_view>

#include <sqlite3.h>

#include "ServerException.hpp"

#include "SpdlogConfig.hpp"

class SqliteQueryResult : public IQueryResult
{
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmt;

public:
    SqliteQueryResult(sqlite3_stmt *stmtPtr)
        : stmt(stmtPtr, sqlite3_finalize) {}

    bool next() override
    {
        return sqlite3_step(stmt.get()) == SQLITE_ROW;
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
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)>
        db{nullptr, sqlite3_close};

    Impl(std::string_view path, std::string_view createDatabaseScriptPath)
        : m_dbPath(path),
          m_createDatabaseScriptPath(createDatabaseScriptPath)
    {
    }

    bool open()
    {
        sqlite3 *dbRaw = nullptr;
        const bool dbExists = std::filesystem::exists(m_dbPath);
        if (sqlite3_open(m_dbPath.c_str(), &dbRaw) != SQLITE_OK)
        {
            SPDLOG_ERROR("Failed to open database: {}", sqlite3_errmsg(dbRaw));
            sqlite3_close(dbRaw);
            throw ServerException("SqliteDatabaseManager", "Failed to open database");
        }

        db.reset(dbRaw);

        if (!dbExists)
        {
            initializeDatabaseSchema();
        }
        SPDLOG_INFO("Database opened successfully");
        return true;
    }

    void close()
    {
        db.reset();
        SPDLOG_INFO("Database closed successfully");
    }

    std::shared_ptr<IQueryResult> executeQuery(std::string_view query, const std::vector<std::string> &params)
    {
        sqlite3_stmt *stmtRaw;
        if (sqlite3_prepare_v2(db.get(), query.data(), -1, &stmtRaw, nullptr) != SQLITE_OK)
        {
            SPDLOG_ERROR("Failed to prepare statement: {}", sqlite3_errmsg(db.get()));
            throw ServerException("SqliteDatabaseManager", "Failed to prepare SQL statement");
        }

        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> stmtGuard(stmtRaw, sqlite3_finalize);

        for (size_t i = 0; i < params.size(); ++i)
        {
            if (sqlite3_bind_text(stmtRaw, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
            {
                SPDLOG_ERROR("Failed to bind parameter at position {}: {}", i + 1, sqlite3_errmsg(db.get()));
                throw ServerException("SqliteDatabaseManager", "Failed to bind SQL parameter");
            }
        }

        return std::make_shared<SqliteQueryResult>(stmtRaw);
    }

    void initializeDatabaseSchema()
    {
        SPDLOG_TRACE("Initializing database schema | using sql script: {}", m_createDatabaseScriptPath);

        std::ifstream sqlFile(m_createDatabaseScriptPath);
        if (!sqlFile.is_open())
        {
            SPDLOG_ERROR("Could not open SQL script file: {}", m_createDatabaseScriptPath);
            throw ServerException("SqliteDatabaseManager", "Could not open SQL script file");
        }

        std::stringstream sqlStream;
        sqlStream << sqlFile.rdbuf();
        std::string sqlCommands = sqlStream.str();

        char *errMsg = nullptr;
        int resultCode = sqlite3_exec(db.get(), sqlCommands.c_str(), nullptr, nullptr, &errMsg);
        std::unique_ptr<char, decltype(&sqlite3_free)> errMsgPtr(errMsg, sqlite3_free);
        if (resultCode != SQLITE_OK)
        {
            SPDLOG_ERROR("Initialize database schema error: {}", errMsg);
            throw ServerException("SqliteDatabaseManager", "Initialize database schema error");
        }
        SPDLOG_INFO("Database schema initialized successfully");
    }
};

SqliteDatabaseManager::SqliteDatabaseManager(std::string_view dbPath, std::string_view createDatabaseScriptPath)
    : pImpl(std::make_unique<Impl>(dbPath, createDatabaseScriptPath))
{
}

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
