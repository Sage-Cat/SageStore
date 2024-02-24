#include <gmock/gmock.h>

#include "RolesRepository.hpp"

class RolesRepositoryMock : public RolesRepository
{
public:
    explicit RolesRepositoryMock(std::shared_ptr<IDatabaseManager> dbManager)
        : RolesRepository(std::move(dbManager)) {}

    MOCK_METHOD(void, add, (const Role &entity), (override));
};
