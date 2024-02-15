#pragma once

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>

namespace Utils
{
    template <typename LayoutItem>
    QVBoxLayout *createVBoxLayout(QWidget *parent = nullptr, std::vector<LayoutItem *> items = {})
    {
        QVBoxLayout *layout = new QVBoxLayout(parent);
        for (auto *item : items)
        {
            if constexpr (std::is_base_of<QLayout, LayoutItem>::value)
            {
                layout->addLayout(static_cast<QLayout *>(item));
            }
            else if constexpr (std::is_base_of<QWidget, LayoutItem>::value)
            {
                layout->addWidget(static_cast<QWidget *>(item));
            }
        }
        return layout;
    }
    template <typename LayoutItem>
    QHBoxLayout *createHBoxLayout(QWidget *parent = nullptr, std::vector<LayoutItem *> items = {})
    {
        QHBoxLayout *layout = new QHBoxLayout(parent);
        for (auto *item : items)
        {
            if constexpr (std::is_base_of<QLayout, LayoutItem>::value)
            {
                layout->addLayout(static_cast<QLayout *>(item));
            }
            else if constexpr (std::is_base_of<QWidget, LayoutItem>::value)
            {
                layout->addWidget(static_cast<QWidget *>(item));
            }
        }
        return layout;
    }

}
