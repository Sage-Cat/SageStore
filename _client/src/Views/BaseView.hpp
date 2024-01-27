#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSpacerItem>
#include "Views/ViewStyles.hpp"

class BaseView : public QWidget
{
    Q_OBJECT

public:
    BaseView(QWidget *parent = nullptr);
    virtual ~BaseView();

protected:
    // setup
    virtual void setupUi();

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
    QSpacerItem *m_additionalUnderStatusSpace;

    // label
    QLabel *m_status;
};
