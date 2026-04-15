#pragma once

#include "Ui/Views/BaseView.hpp"
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QWidget>

#include "Ui/DisplayData/User.hpp"

class UsersManagementViewModel;

class UsersView : public BaseView {
    Q_OBJECT
public:
    explicit UsersView(UsersManagementViewModel &viewModel, QWidget *parent = nullptr);

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onItemChanged(QTableWidgetItem *item);
    void onUsersChanged();
    void onSelectionChanged();

private:
    void fillTable(const QVector<DisplayData::User> &users);
    void persistRow(int row);
    void updateActionButtons();
    void updateStatus(int userCount);

private:
    UsersManagementViewModel &m_viewModel;
    bool m_isSyncingTable{false};
};
