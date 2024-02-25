#include <gmock/gmock.h>

#include <memory>

#include "DatabaseManagerMock.hpp"
#include "Database/RepositoryManager.hpp"

#include "Database/Entities/User.hpp"
#include "Database/Entities/Role.hpp"

class SqliteDatabaseManager;

class RepositoryManagerMock : public RepositoryManager
{
public:
    RepositoryManagerMock() : RepositoryManager(std::make_shared<DatabaseManagerMock>()) {}

    MOCK_METHOD(std::shared_ptr<IRepository<User>>, getUsersRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Role>>, getRolesRepository, (), (override));
};
