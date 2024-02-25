#pragma once

#include <QMessageBox>

class QMessageBoxMock : public QMessageBox
{
    int exec() override
    {
        emit buttonClicked(nullptr);
        return 0;
    }
};