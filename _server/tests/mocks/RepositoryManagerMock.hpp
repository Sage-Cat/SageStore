#include <gmock/gmock.h>

#include "RepositoryManager.hpp"

class SqliteDatabaseManager;

class RepositoryManagerMock : public RepositoryManager
{
public:
    RepositoryManagerMock();

    MOCK_METHOD(auto, getUsersRepository, (), (override));
    MOCK_METHOD(auto, getRolesRepository, (), (override));
};
