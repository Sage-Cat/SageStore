#pragma once

#include "Ui/Views/BaseView.hpp"
#include <QTableWidget>
#include <QWidget>

#include "Ui/DisplayData/User.hpp"

class UsersManagementViewModel;

class UsersView : public BaseView {
    Q_OBJECT
public:
    explicit UsersView(UsersManagementViewModel &viewModel, QWidget *parent = nullptr);

private slots:
    void onDeleteButtonClicked();
    void onEditButtonClicked();
    void onUsersChanged();

private:
    void fillTable(const QVector<DisplayData::User> &users);

private:
    UsersManagementViewModel &m_viewModel;
};
