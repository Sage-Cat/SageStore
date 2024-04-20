#include <gmock/gmock.h>

#include <memory>

#include "DatabaseManagerMock.hpp"
#include "Database/RepositoryManager.hpp"

#include "common/Entities/User.hpp"
#include "common/Entities/Role.hpp"

class SqliteDatabaseManager;

class RepositoryManagerMock : public RepositoryManager
{
public:
    RepositoryManagerMock() : RepositoryManager(std::make_shared<DatabaseManagerMock>()) {}

    MOCK_METHOD(std::shared_ptr<IRepository<User>>, getUserRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Role>>, getRoleRepository, (), (override));
};
