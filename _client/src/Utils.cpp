#include "Utils.hpp"

QVBoxLayout *Utils::createVBoxLayout(QWidget *parent, std::vector<QWidget *> widgets)
{
    QVBoxLayout *layout = new QVBoxLayout();
    for (auto *widget : widgets)
    {
        layout->addWidget(widget);
    }
    return layout;
}

QHBoxLayout *Utils::createHBoxLayout(QWidget *parent, std::vector<QWidget *> widgets)
{
    QHBoxLayout *layout = new QHBoxLayout();
    for (auto *widget : widgets)
    {
        layout->addWidget(widget);
    }
    return layout;
}