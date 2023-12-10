#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSpacerItem>

const int ADDIT_SPACE_WIDTH = 100;
const int ADDIT_SPACE_HEIGHT = 50;

class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    BaseWidget(QWidget *parent = nullptr);

private:
    // setup
    void setupUi();

protected:
    // layputs
    QVBoxLayout * m_MainLayout;
    QHBoxLayout * m_ButtonRow;
    QHBoxLayout * m_AdditionalInfo;
    // buttons
    QPushButton * m_AddButton;
    QPushButton * m_DeleteButton;
    QPushButton * m_EditButton;
    // data table
    QTableWidget * m_DataTable;
    // additional space
    QSpacerItem * m_AdditionalButtonSpace;
    QSpacerItem * m_AdditionalUnderTableSpace;
    // label
    QLabel * m_Status;
};
