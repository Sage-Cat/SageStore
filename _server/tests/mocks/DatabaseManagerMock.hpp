#include <gmock/gmock.h>

#include "Database/IDatabaseManager.hpp"

class QueryResultMock : public IQueryResult
{
public:
    MOCK_METHOD(bool, next, (), (override));
    MOCK_METHOD(std::string, getString, (int columnIndex), (const, override));
};

class DatabaseManagerMock : public IDatabaseManager
{
public:
    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(void, close, (), (override));
    MOCK_METHOD(std::shared_ptr<IQueryResult>, executeQuery, (std::string_view query, const std::vector<std::string> &params), (override));
};