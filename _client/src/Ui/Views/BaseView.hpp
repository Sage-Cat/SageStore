#pragma once

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSpacerItem>
#include "Ui/Views/ViewStyles.hpp"

class BaseView : public QWidget
{
    Q_OBJECT

public:
    BaseView(QWidget *parent = nullptr);
    virtual ~BaseView();

    template<typename Layout, class = std::enable_if_t<is_QHBoxLayout_QVBoxLayout>>
    Layout* createBoxLayout(QVector<QWidget*> widgets, QWidget* parent = nullptr);

protected:
    // setup
    virtual void setupUi();

    // layputs
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonRow;
    QHBoxLayout *m_additionalInfo;

    //GroupBoxes
    //QGroupBox *m_buttonGroupBox;
    //QGroupBox *m_infoGroupBox;

    // buttons
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QPushButton *m_editButton;

    // data table
    QTableWidget *m_dataTable;

    // label
    QLabel *m_status;

    //spacerItems
    QSpacerItem* m_buttonRowSpacerItem;
    QSpacerItem* m_additionalInfoSpacerItem;
};
