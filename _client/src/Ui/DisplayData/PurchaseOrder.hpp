#pragma once

#include <QString>

namespace DisplayData {
struct PurchaseOrder {
    static constexpr const size_t VAR_COUNT = 7;

    QString id{};
    QString date{};
    QString userId{};
    QString username{};
    QString supplierId{};
    QString supplier{};
    QString status{};
    };
} 