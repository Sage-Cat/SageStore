#pragma once

#include <QVBoxLayout>

namespace Utils
{
    QVBoxLayout *createLayout(std::vector<QWidget *> widgets);
}