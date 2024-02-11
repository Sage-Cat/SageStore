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

    //Methods with buttons
    void addButton(QPushButton* buttonToAdd);
    void addButton(const std::vector<QPushButton*> buttonsToAdd);

    void removeButton(QPushButton* buttonToRemove);
    void removeButton(const std::vector<QPushButton*> buttonsToRemove);

    void setButtonLayout(QHBoxLayout* buttonlayout);
    void setButtonLayout(QGroupBox* buttonGroupBox);
    const QLayout* getButtonLayout();

    //Methods with dataTable
    //void setDataTable(QTableWidget* dataTable);
    const QTableWidget* getDataTable();

    //Methods with AdditionalInfo
    void setStatus(const char* status);
    QLabel* getStatus();

    //Methods with UnderSpace
    void addUnderSpace();
protected:
    // setup
    virtual void setupUi();

    // layputs
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonRow;
    QHBoxLayout *m_additionalInfo;

    //GroupBoxes
    QGroupBox *buttonGroupBox;
    QGroupBox *infoGroupBox;

    // buttons
    QPushButton *m_addButton;
    QPushButton *m_deleteButton;
    QPushButton *m_editButton;

    // data table
    QTableWidget *m_dataTable;

    // label
    QLabel *m_status;
};
