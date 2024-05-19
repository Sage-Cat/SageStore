#pragma once

#include <QHBoxLayout>
#include <QLayout>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <concepts>
#include <variant>

namespace Utils {
template <typename T>
concept LayoutCompatible = std::is_base_of_v<QLayout, T>;

template <LayoutCompatible LayoutType>
LayoutType *createLayout(const QVector<std::variant<QWidget *, QLayout *, QLayoutItem *>> &items,
                         QWidget *parent = nullptr);

} // namespace Utils
