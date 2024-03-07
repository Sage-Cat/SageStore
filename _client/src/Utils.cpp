#include "Utils.hpp"

namespace Utils
{

    template <LayoutCompatible LayoutType>
    LayoutType *createLayout(QWidget *parent, const QVector<std::variant<QWidget *, QLayout *, QLayoutItem*>> &items)
    {
        auto *layout = new LayoutType(parent);
        for (const auto &item : items)
        {
            std::visit([&layout](auto &&arg)
                       {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, QWidget*>) {
                    layout->addWidget(arg);
                } else if constexpr (std::is_same_v<T, QLayout*>) {
                    layout->addLayout(arg);
                }else if constexpr (std::is_same_v<T, QLayoutItem*>) {
                    layout->addItem(arg);
                } },
                       item);
        }
        return layout;
    }

    // Explicit template instantiations
    template QVBoxLayout *createLayout<QVBoxLayout>(QWidget *, const QVector<std::variant<QWidget *, QLayout *, QLayoutItem*>> &);
    template QHBoxLayout *createLayout<QHBoxLayout>(QWidget *, const QVector<std::variant<QWidget *, QLayout *, QLayoutItem*>> &);

} // namespace Utils
