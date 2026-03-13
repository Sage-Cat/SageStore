#pragma once

#include <QString>

namespace DisplayData {
struct ProductType {
    static constexpr const size_t VAR_COUNT = 8;

    QString id{};
    QString code{};
    QString barcode{};
    QString name{};
    QString description{};
    QString lastPrice{};
    QString unit{};
    QString isImported{};
};
} // namespace DisplayData
