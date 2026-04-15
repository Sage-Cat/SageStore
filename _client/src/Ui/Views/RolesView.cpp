#include "Ui/Views/RolesView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QDateTime>
#include <QItemSelectionModel>
#include <QTableWidgetItem>

#include "Ui/ViewModels/UsersManagementViewModel.hpp"

#include "common/SpdlogConfig.hpp"

RolesView::RolesView(UsersManagementViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("RolesView::RolesView");

    m_addButton->setObjectName("rolesAddButton");
    m_deleteButton->setObjectName("rolesDeleteButton");
    m_dataTable->setObjectName("rolesTable");
    m_status->setObjectName("rolesStatusLabel");

    enableInlineEditing();
    m_dataTable->setSortingEnabled(false);

    connect(m_addButton, &QPushButton::clicked, this, &RolesView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &RolesView::onDeleteButtonClicked);
    connect(&m_viewModel, &UsersManagementViewModel::rolesChanged, this, &RolesView::onRolesChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &RolesView::onSelectionChanged);
    connect(m_dataTable, &QTableWidget::itemChanged, this, &RolesView::onItemChanged);

    updateActionButtons();
    updateStatus(0);
}

void RolesView::onAddButtonClicked()
{
    DisplayData::Role role;
    role.name = tr("New role %1")
                    .arg(QDateTime::currentDateTimeUtc().toString(QStringLiteral("hhmmsszzz")))
                    .toStdString();
    m_viewModel.createRole(role);
}

void RolesView::onDeleteButtonClicked()
{
    const auto selectedItems = m_dataTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        const int row = selectedItems.first()->row();
        m_viewModel.deleteRole(m_dataTable->item(row, 0)->text());
    } else {
        SPDLOG_WARN("No role selected for deletion.");
    }
}

void RolesView::onRolesChanged()
{
    const auto roles = m_viewModel.roles();
    fillTable(roles);
    updateStatus(roles.size());
}

void RolesView::onSelectionChanged() { updateActionButtons(); }

void RolesView::onItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr || item->column() != 1) {
        return;
    }

    const QString previousValue = item->data(Qt::UserRole).toString();
    const QString currentValue = item->text().trimmed();
    if (currentValue.isEmpty()) {
        restoreItemText(m_dataTable, item, previousValue);
        emit errorOccurred(tr("Role name is required."));
        return;
    }
    if (restoreIfUnchanged(m_dataTable, item, currentValue)) {
        return;
    }

    {
        const QSignalBlocker blocker(m_dataTable);
        item->setData(Qt::UserRole, currentValue);
    }

    DisplayData::Role role;
    role.id = m_dataTable->item(item->row(), 0)->text().toStdString();
    role.name = currentValue.toStdString();
    m_viewModel.editRole(role);
}

void RolesView::fillTable(const QVector<DisplayData::Role> &roles)
{
    m_isSyncingTable = true;
    m_dataTable->setUpdatesEnabled(false);
    m_dataTable->clearContents();
    m_dataTable->setRowCount(roles.size());
    m_dataTable->setColumnCount(2);
    m_dataTable->setHorizontalHeaderLabels({tr("ID"), tr("Role name")});

    for (int row = 0; row < roles.size(); ++row) {
        const auto &role = roles[row];
        m_dataTable->setItem(row, 0, createReadOnlyTableItem(QString::fromStdString(role.id)));
        m_dataTable->setItem(row, 1, createEditableTableItem(QString::fromStdString(role.name)));
    }

    m_dataTable->setColumnHidden(0, true);
    m_dataTable->setUpdatesEnabled(true);
    m_isSyncingTable = false;
    updateActionButtons();
}

void RolesView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_deleteButton->setEnabled(hasSelection);
}

void RolesView::updateStatus(int roleCount)
{
    m_status->setText(tr("Status: %1 roles available").arg(QString::number(roleCount)));
}
