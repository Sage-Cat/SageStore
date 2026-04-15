#pragma once

#include <QTableWidgetItem>
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
    void onItemChanged(QTableWidgetItem *item);
    void onRolesChanged();
    void onSelectionChanged();

private:
    void fillTable(const QVector<DisplayData::Role> &roles);
    void updateActionButtons();
    void updateStatus(int roleCount);

    UsersManagementViewModel &m_viewModel;
    bool m_isSyncingTable{false};
};
