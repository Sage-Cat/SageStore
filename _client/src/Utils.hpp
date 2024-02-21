#pragma once

#include <variant>

#include <QLayout>
#include <QWidget>
#include <QVector>

namespace Utils
{
    template <typename LayoutType>
        requires std::is_base_of_v<QLayout, LayoutType>
    LayoutType *createLayout(QWidget *parent, const QVector<std::variant<QWidget *, QLayout *>> &items);
} // namespace Utils
