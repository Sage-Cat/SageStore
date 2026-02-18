#pragma once

#include <QString>

namespace DisplayData {
struct User {
    static constexpr const size_t VAR_COUNT = 4;

    QString id{};
    QString username{};
    QString password{};
    QString roleId{};
    QString roleName{};
};
} // namespace DisplayData
