#include "DatabaseManager.hpp"

#include "ServerException.hpp"

#include "SpdlogConfig.hpp"

DatabaseManager::DatabaseManager(std::string_view dbPath)
    : m_dbPath(dbPath),
      m_db(nullptr, sqlite3_close),
      m_stmt(nullptr, sqlite3_finalize)
{
    SPDLOG_TRACE("DatabaseManager::DatabaseManager");
}

bool DatabaseManager::open()
{
    SPDLOG_TRACE("DatabaseManager::open");
    sqlite3 *dbTemp = nullptr;
    auto resultCode = sqlite3_open(m_dbPath.c_str(), &dbTemp);
    m_db.reset(dbTemp);
    checkSQLiteResult(resultCode, "Open database");
    return resultCode == SQLITE_OK;
}

void DatabaseManager::close()
{
    SPDLOG_TRACE("DatabaseManager::close");
    finalizeStatement();
    m_db.reset();
}

bool DatabaseManager::executeSQL(std::string_view query)
{
    SPDLOG_TRACE("DatabaseManager::executeSQL");
    char *errMsg = nullptr;
    int resultCode = sqlite3_exec(m_db.get(), query.data(), nullptr, nullptr, &errMsg);
    std::unique_ptr<char, decltype(&sqlite3_free)> errMsgPtr(errMsg, sqlite3_free); // Ensure errMsg is freed
    checkSQLiteResult(resultCode, "Execute SQL");
    return resultCode == SQLITE_OK;
}

void DatabaseManager::prepareStatement(std::string_view query)
{
    SPDLOG_TRACE("DatabaseManager::prepareStatement - {}", query);
    finalizeStatement(); // Ensure any previous statement is finalized
    sqlite3_stmt *stmtTemp = nullptr;
    auto resultCode = sqlite3_prepare_v2(m_db.get(), query.data(), -1, &stmtTemp, nullptr);
    m_stmt.reset(stmtTemp);
    checkSQLiteResult(resultCode, "Prepare statement");
}

void DatabaseManager::bind(int index, std::string_view value)
{
    SPDLOG_TRACE("DatabaseManager::bind | index = {} | value = {}", index, value);
    auto resultCode = sqlite3_bind_text(m_stmt.get(), index, value.data(), -1, SQLITE_TRANSIENT);
    checkSQLiteResult(resultCode, "Bind value");
}

bool DatabaseManager::step()
{
    SPDLOG_TRACE("DatabaseManager::step");
    int resultCode = sqlite3_step(m_stmt.get());
    if (resultCode == SQLITE_ROW)
    {
        return true; // More rows available
    }
    else if (resultCode == SQLITE_DONE)
    {
        return false; // No more rows
    }
    else
    {
        checkSQLiteResult(resultCode, "Step through statement");
        return false;
    }
}

std::string DatabaseManager::columnText(int index)
{
    SPDLOG_TRACE("DatabaseManager::columnText");
    const unsigned char *text = sqlite3_column_text(m_stmt.get(), index);
    if (text != nullptr)
    {
        return std::string(reinterpret_cast<const char *>(text));
    }
    return "";
}

int DatabaseManager::columnInt(int index)
{
    return sqlite3_column_int(m_stmt.get(), index);
}

void DatabaseManager::finalizeStatement()
{
    m_stmt.reset();
}

void DatabaseManager::checkSQLiteResult(int resultCode, std::string_view context)
{
    if (resultCode != SQLITE_OK)
    {
        SPDLOG_ERROR("{} error: {}", context, sqlite3_errmsg(m_db.get()));
        throw ServerException("DatabaseManager", std::string(context) + " error");
    }
}
