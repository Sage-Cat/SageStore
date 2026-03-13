#pragma once

#include <QString>

namespace DisplayData {
struct InventoryStock {
    static constexpr const size_t VAR_COUNT = 5;

    QString id{};
    QString productTypeId{};
    QString productLabel{};
    QString quantityAvailable{};
    QString employeeId{};
};
} // namespace DisplayData
