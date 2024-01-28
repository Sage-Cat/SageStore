#pragma once

#include <QDialog>

class BaseDialog : public QDialog
{
    Q_OBJECT

public:
    BaseDialog(QDialog *parent = nullptr);
    virtual ~BaseDialog();

protected:
};
