#pragma once

#include <QDialog>
#include <QVBoxLayout>

class BaseDialog : public QDialog
{
    Q_OBJECT

public:
    BaseDialog(QDialog *parent = nullptr);
    virtual ~BaseDialog();

protected:
};
