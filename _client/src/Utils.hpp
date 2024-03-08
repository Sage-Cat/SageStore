#pragma once

#include <concepts>
#include <variant>
#include <QVector>
#include <QWidget>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace Utils
{
    template <typename T>
    concept LayoutCompatible = std::is_base_of_v<QLayout, T>;

    template <LayoutCompatible LayoutType>
    LayoutType *createLayout(QWidget *parent, const QVector<std::variant<QWidget *, QLayout *, QLayoutItem*>> &items);

} // namespace Utils
