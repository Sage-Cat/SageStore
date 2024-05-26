#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

class IQueryResult {
public:
    virtual ~IQueryResult() = default;

    virtual bool next()                                  = 0;
    virtual std::string getString(int columnIndex) const = 0;
};

/**
 * \brief Interface for SQL databases, like SQLite or Postgre
 *
 * Example of usage:
 *  executeQuery("INSERT INTO Users (username, password) VALUES (?, ?);",
 * {"username1", "password1"});
 */
class IDatabaseManager {
public:
    virtual ~IDatabaseManager() = default;

    virtual bool open()                                                                        = 0;
    virtual void close()                                                                       = 0;
    virtual std::shared_ptr<IQueryResult> executeQuery(std::string_view query,
                                                       const std::vector<std::string> &params) = 0;
};
