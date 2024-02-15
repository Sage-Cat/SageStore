#pragma once

#include <QVBoxLayout>

namespace Utils
{
    QVBoxLayout *createVBoxLayout(QWidget *parent = nullptr, std::vector<QWidget *> widgets);
    QHBoxLayout *createHBoxLayout(QWidget *parent = nullptr, std::vector<QWidget *> widgets);
}