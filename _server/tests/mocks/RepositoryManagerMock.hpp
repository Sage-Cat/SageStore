#include <gmock/gmock.h>

#include <memory>

#include "Database/RepositoryManager.hpp"
#include "DatabaseManagerMock.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"

class SqliteDatabaseManager;

class RepositoryManagerMock : public RepositoryManager {
public:
    RepositoryManagerMock()
        : RepositoryManager(std::make_shared<::testing::NiceMock<DatabaseManagerMock>>())
    {
    }

    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::User>>, getUserRepository, (),
                (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Role>>, getRoleRepository, (),
                (override));
};
