#include <gmock/gmock.h>

#include <optional>
#include <vector>

#include "Database/IRepository.hpp"

template <typename T>
class RepositoryMock : public IRepository<T>
{
public:
    RepositoryMock() = default;

    MOCK_METHOD(void, add, (const T &entity), (override));
    MOCK_METHOD(void, update, (const T &entity), (override));
    MOCK_METHOD(void, deleteResource, (const std::string &id), (override));
    MOCK_METHOD(std::vector<T>, getByField, (const std::string &fieldName, const std::string &value), (const, override));
    MOCK_METHOD(std::vector<T>, getAll, (), (const, override));
};
