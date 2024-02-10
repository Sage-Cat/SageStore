#include "Utils.hpp"

QVBoxLayout *Utils::createLayout(std::vector<QWidget *> widgets)
{
    QVBoxLayout *layout = new QVBoxLayout();
    for (auto *widget : widgets)
    {
        layout->addWidget(widget);
    }
    return layout;
}