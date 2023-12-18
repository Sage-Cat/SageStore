#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSpacerItem>
#include "Constants.cpp"

class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    BaseWidget(QWidget *parent = nullptr);
    ~BaseWidget();

protected:
    // setup
    void virtual setupUi();

    // layputs
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonRow;
    QHBoxLayout *m_additionalInfo;

    // buttons
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QPushButton *m_editButton;

    // data table
    QTableWidget *m_dataTable;

    // additional space
    QSpacerItem *m_additionalButtonSpace;
    QSpacerItem *m_additionalUnderTableSpace;

    // label
    QLabel *m_status;
};
