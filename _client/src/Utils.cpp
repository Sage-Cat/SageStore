#include "Utils.hpp"

namespace Utils {

template <LayoutCompatible LayoutType>
LayoutType *createLayout(const QVector<std::variant<QWidget *, QLayout *, QLayoutItem *>> &items,
                         QWidget *parent)
{
    auto *layout = new LayoutType(parent);
    for (const auto &item : items) {
        std::visit(
            [&layout](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, QWidget *>) {
                    layout->addWidget(arg);
                } else if constexpr (std::is_same_v<T, QLayout *>) {
                    layout->addLayout(arg);
                } else if constexpr (std::is_same_v<T, QLayoutItem *>) {
                    layout->addItem(arg);
                }
            },
            item);
    }
    return layout;
}

// Explicit template instantiations
template QVBoxLayout *
createLayout<QVBoxLayout>(const QVector<std::variant<QWidget *, QLayout *, QLayoutItem *>> &,
                          QWidget *);
template QHBoxLayout *
createLayout<QHBoxLayout>(const QVector<std::variant<QWidget *, QLayout *, QLayoutItem *>> &,
                          QWidget *);

} // namespace Utils
