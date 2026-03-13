#pragma once

#include <QVector>

#include "Ui/DisplayData/Entities.hpp"
#include "Ui/Views/BaseView.hpp"

class UsersManagementViewModel;

class RolesView : public BaseView {
    Q_OBJECT

public:
    explicit RolesView(UsersManagementViewModel &viewModel, QWidget *parent = nullptr);

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onEditButtonClicked();
    void onRolesChanged();
    void onSelectionChanged();

private:
    void fillTable(const QVector<DisplayData::Role> &roles);
    bool showRoleDialog(DisplayData::Role &role, const QString &title);
    void updateActionButtons();
    void updateStatus(int roleCount);

    UsersManagementViewModel &m_viewModel;
};
