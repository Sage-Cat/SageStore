#include "Ui/Views/RolesView.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QItemSelectionModel>
#include <QLineEdit>
#include <QTableWidgetItem>

#include "Ui/ViewModels/UsersManagementViewModel.hpp"

#include "common/SpdlogConfig.hpp"

RolesView::RolesView(UsersManagementViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("RolesView::RolesView");

    m_addButton->setObjectName("rolesAddButton");
    m_editButton->setObjectName("rolesEditButton");
    m_deleteButton->setObjectName("rolesDeleteButton");
    m_dataTable->setObjectName("rolesTable");
    m_status->setObjectName("rolesStatusLabel");

    connect(m_addButton, &QPushButton::clicked, this, &RolesView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &RolesView::onDeleteButtonClicked);
    connect(m_editButton, &QPushButton::clicked, this, &RolesView::onEditButtonClicked);
    connect(&m_viewModel, &UsersManagementViewModel::rolesChanged, this, &RolesView::onRolesChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &RolesView::onSelectionChanged);

    updateActionButtons();
    updateStatus(0);
}

void RolesView::onAddButtonClicked()
{
    DisplayData::Role role;
    if (showRoleDialog(role, tr("Create role"))) {
        m_viewModel.createRole(role);
    }
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

void RolesView::onEditButtonClicked()
{
    const auto selectedItems = m_dataTable->selectedItems();
    if (selectedItems.isEmpty()) {
        SPDLOG_WARN("No role selected for editing.");
        return;
    }

    const int row = selectedItems.first()->row();
    DisplayData::Role role;
    role.id   = m_dataTable->item(row, 0)->text().toStdString();
    role.name = m_dataTable->item(row, 1)->text().toStdString();

    if (showRoleDialog(role, tr("Edit role"))) {
        m_viewModel.editRole(role);
    }
}

void RolesView::onRolesChanged()
{
    const auto roles = m_viewModel.roles();
    fillTable(roles);
    updateStatus(roles.size());
}

void RolesView::onSelectionChanged() { updateActionButtons(); }

void RolesView::fillTable(const QVector<DisplayData::Role> &roles)
{
    m_dataTable->clearContents();
    m_dataTable->setRowCount(roles.size());
    m_dataTable->setColumnCount(2);
    m_dataTable->setHorizontalHeaderLabels({"ID", "Role Name"});

    for (int row = 0; row < roles.size(); ++row) {
        const auto &role = roles[row];
        m_dataTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(role.id)));
        m_dataTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(role.name)));
    }

    m_dataTable->setColumnHidden(0, true);
    updateActionButtons();
}

bool RolesView::showRoleDialog(DisplayData::Role &role, const QString &title)
{
    QDialog dialog(this);
    dialog.setWindowTitle(title);
    dialog.setObjectName("roleDialog");
    dialog.setMinimumWidth(360);

    auto *layout    = new QFormLayout(&dialog);
    auto *nameField = new QLineEdit(QString::fromStdString(role.name), &dialog);
    nameField->setObjectName("roleNameField");

    layout->addRow(tr("Role name"), nameField);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, [this, &dialog, nameField]() {
        if (nameField->text().trimmed().isEmpty()) {
            emit errorOccurred(tr("Role name is required."));
            return;
        }

        dialog.accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }

    role.name = nameField->text().trimmed().toStdString();
    return true;
}

void RolesView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_editButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}

void RolesView::updateStatus(int roleCount)
{
    m_status->setText(tr("Status: %1 roles available").arg(QString::number(roleCount)));
}
