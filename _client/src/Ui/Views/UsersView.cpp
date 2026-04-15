#include "Ui/Views/UsersView.hpp"

#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QComboBox>
#include <QDateTime>
#include <QItemSelectionModel>
#include <QTableWidgetItem>

#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "common/SpdlogConfig.hpp"

UsersView::UsersView(UsersManagementViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("UsersView::UsersView");

    m_addButton->setObjectName("usersAddButton");
    m_deleteButton->setObjectName("usersDeleteButton");
    m_dataTable->setObjectName("usersTable");
    m_status->setObjectName("usersStatusLabel");

    enableInlineEditing();
    m_dataTable->setSortingEnabled(false);
    installComboDelegate(
        m_dataTable, 4, QStringLiteral("usersRoleCombo"),
        [this](const QModelIndex &) {
            QVector<TableComboChoice> choices;
            const auto roles = m_viewModel.roles();
            choices.reserve(roles.size());
            for (const auto &role : roles) {
                choices.push_back({QString::fromStdString(role.name), QString::fromStdString(role.id)});
            }
            return choices;
        },
        [this](const QModelIndex &index, const TableComboChoice &choice) {
            if (m_isSyncingTable) {
                return;
            }

            auto *roleIdItem = m_dataTable->item(index.row(), 3);
            if (roleIdItem == nullptr || choice.value.toString().isEmpty()) {
                return;
            }

            {
                const QSignalBlocker blocker(m_dataTable);
                roleIdItem->setText(choice.value.toString());
                roleIdItem->setData(Qt::UserRole, choice.value);
            }
            persistRow(index.row());
        });

    connect(m_addButton, &QPushButton::clicked, this, &UsersView::onAddButtonClicked);
    connect(m_deleteButton, &QPushButton::clicked, this, &UsersView::onDeleteButtonClicked);
    connect(&m_viewModel, &UsersManagementViewModel::usersChanged, this,
            &UsersView::onUsersChanged);
    connect(m_dataTable->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &UsersView::onSelectionChanged);
    connect(m_dataTable, &QTableWidget::itemChanged, this, &UsersView::onItemChanged);

    updateActionButtons();
    updateStatus(0);
}

void UsersView::onAddButtonClicked()
{
    const auto roles = m_viewModel.roles();
    if (roles.isEmpty()) {
        emit errorOccurred(tr("Create at least one role before managing users."));
        return;
    }

    const QString token = QDateTime::currentDateTimeUtc().toString(QStringLiteral("hhmmsszzz"));
    DisplayData::User user;
    user.username = QStringLiteral("user-%1").arg(token);
    user.password = QStringLiteral("changeme");
    user.roleId = QString::fromStdString(roles.first().id);
    user.roleName = QString::fromStdString(roles.first().name);
    m_viewModel.addUser(user);
}

void UsersView::onDeleteButtonClicked()
{
    SPDLOG_TRACE("UsersView::onDeleteButtonClicked");
    const auto selectedItems = m_dataTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        const int row  = selectedItems.first()->row();
        const QString userId = m_dataTable->item(row, 0)->text();
        m_viewModel.deleteUser(userId);
    } else {
        SPDLOG_WARN("No user selected for deletion.");
    }
}

void UsersView::onUsersChanged()
{
    SPDLOG_TRACE("UsersView::onUsersChanged");
    const auto users = m_viewModel.users();
    fillTable(users);
    updateStatus(users.size());
}

void UsersView::onSelectionChanged() { updateActionButtons(); }

void UsersView::onItemChanged(QTableWidgetItem *item)
{
    if (m_isSyncingTable || item == nullptr) {
        return;
    }

    if (item->column() == 1) {
        const QString currentValue = item->text().trimmed();
        if (currentValue.isEmpty()) {
            restoreItemText(m_dataTable, item, item->data(Qt::UserRole).toString());
            emit errorOccurred(tr("Username is required."));
            return;
        }
        if (restoreIfUnchanged(m_dataTable, item, currentValue)) {
            return;
        }
        if (item->text() != currentValue) {
            restoreItemText(m_dataTable, item, currentValue);
        }
        {
            const QSignalBlocker blocker(m_dataTable);
            item->setData(Qt::UserRole, currentValue);
        }
    } else if (item->column() == 2) {
        if (trackedItemText(item) == item->text()) {
            return;
        }
        const QSignalBlocker blocker(m_dataTable);
        item->setData(Qt::UserRole, item->text());
    } else {
        return;
    }

    persistRow(item->row());
}

void UsersView::persistRow(int row)
{
    if (row < 0) {
        return;
    }

    DisplayData::User user;
    user.id = m_dataTable->item(row, 0)->text();
    user.username = m_dataTable->item(row, 1)->text().trimmed();
    user.password = m_dataTable->item(row, 2)->text();
    user.roleId = m_dataTable->item(row, 3)->text();
    user.roleName = m_dataTable->item(row, 4)->text();

    if (user.username.isEmpty()) {
        emit errorOccurred(tr("Username is required."));
        return;
    }

    if (user.id.isEmpty()) {
        if (user.password.isEmpty()) {
            emit errorOccurred(tr("Password is required for new users."));
            return;
        }
        m_viewModel.addUser(user);
    } else {
        m_viewModel.editUser(user);
    }
}

void UsersView::fillTable(const QVector<DisplayData::User> &users)
{
    SPDLOG_TRACE("UsersView::fillTable");
    m_isSyncingTable = true;
    m_dataTable->clearContents();
    m_dataTable->setRowCount(users.size());
    m_dataTable->setColumnCount(DisplayData::User::VAR_COUNT);
    const QStringList headers = {tr("ID"), tr("Username"), tr("Password"), tr("Role ID"),
                                 tr("Role")};
    m_dataTable->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < users.size(); ++row) {
        const auto &user = users[row];
        m_dataTable->setItem(row, 0, createReadOnlyTableItem(user.id));
        m_dataTable->setItem(row, 1, createEditableTableItem(user.username));
        m_dataTable->setItem(row, 2, createEditableTableItem({}));
        m_dataTable->setItem(row, 3, createReadOnlyTableItem(user.roleId));
        m_dataTable->setItem(row, 4, createComboTableItem(user.roleName, user.roleId));
    }

    m_dataTable->setColumnHidden(0, true);
    m_dataTable->setColumnHidden(3, true);
    refreshPersistentComboEditors(m_dataTable);
    m_isSyncingTable = false;
    updateActionButtons();
}

void UsersView::updateActionButtons()
{
    const bool hasSelection = !m_dataTable->selectedItems().isEmpty();
    m_deleteButton->setEnabled(hasSelection);
}

void UsersView::updateStatus(int userCount)
{
    m_status->setText(tr("Status: %1 users available").arg(QString::number(userCount)));
}
