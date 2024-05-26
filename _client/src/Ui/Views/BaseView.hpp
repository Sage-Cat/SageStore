#pragma once

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

#include "Ui/Views/ViewStyles.hpp"
#include "Utils.hpp"

class BaseViewModel;

class BaseView : public QWidget {
    Q_OBJECT

public:
    BaseView(BaseViewModel &viewModel, QWidget *parent = nullptr);
    virtual ~BaseView();

signals:
    void errorOccurred(const QString &errorMessage);

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

    // label
    QLabel *m_status;

    // spacerItems
    QSpacerItem *m_buttonRowSpacerItem;
    QSpacerItem *m_additionalInfoSpacerItem;
};
