#include <gmock/gmock.h>

#include "Database/UsersRepository.hpp"

class UsersRepositoryMock : public UsersRepository
{
public:
    explicit UsersRepositoryMock(std::shared_ptr<IDatabaseManager> dbManager)
        : UsersRepository(std::move(dbManager)) {}

    MOCK_METHOD(void, add, (const User &entity), (override));
    MOCK_METHOD(std::optional<User>, getByUsername, (const std::string &username) const, (override));
};