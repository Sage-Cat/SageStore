#pragma once

#include <optional>

#include "DataSpecs.hpp"

template <typename T>
class IRepository
{
public:
    virtual ~IRepository() = default;

    virtual void add(const T &entity) = 0;
    virtual void update(const T &entity) = 0;
    virtual void deleteResource(const std::string &id) = 0;
    virtual std::optional<T> getById(const std::string &id) const = 0;
    virtual std::vector<T> getAll() const = 0;
};
