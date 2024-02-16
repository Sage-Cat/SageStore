#include "Utils.hpp"

namespace Utils
{

    template <typename LayoutType>
        requires std::is_base_of_v<QLayout, LayoutType>
    LayoutType *createLayout(QWidget *parent, const QVector<std::variant<QWidget *, QLayout *>> &items)
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
            } },
                       item);
        }
        return layout;
    }

    // Explicit instantiation
    template QVBoxLayout *Utils::createLayout<QVBoxLayout>(QWidget *, const QVector<std::variant<QWidget *, QLayout *>> &);
    template QHBoxLayout *Utils::createLayout<QHBoxLayout>(QWidget *, const QVector<std::variant<QWidget *, QLayout *>> &);

} // namespace Utils
