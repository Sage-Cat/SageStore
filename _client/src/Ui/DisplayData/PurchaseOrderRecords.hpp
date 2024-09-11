#pragma once

#include <QString>

namespace DisplayData {
struct PurchaseOrderRecord {
    static constexpr const size_t VAR_COUNT = 10;

    QString id{};
    QString productTypeId{};
    QString orderId{};
    QString code{};
    QString name{};
    QString quantity{};
    QString unit{};
    QString price{};
    QString barcode{};
    QString description{};
    QString isImported{};
    };
} 