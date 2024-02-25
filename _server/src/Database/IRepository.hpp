#pragma once

#include "DataSpecs.hpp"

template <typename T>
class IRepository
{
public:
    virtual ~IRepository() = default;

    virtual void add(const T &entity) = 0;
    virtual void update(const T &entity) = 0;
    virtual void deleteResource(const std::string &id) = 0;
    virtual std::vector<T> getByField(const std::string &fieldName, const std::string &value) const = 0;
    virtual std::vector<T> getAll() const = 0;
};
