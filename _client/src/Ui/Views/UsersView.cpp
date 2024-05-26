#include "Ui/Views/UsersView.hpp"
#include "Ui/ViewModels/UsersManagementViewModel.hpp"
#include "common/SpdlogConfig.hpp"

UsersView::UsersView(UsersManagementViewModel &viewModel, QWidget *parent)
    : BaseView(viewModel, parent), m_viewModel(viewModel)
{
    SPDLOG_TRACE("UsersView::UsersView");

    connect(m_deleteButton, &QPushButton::clicked, this, &UsersView::onDeleteButtonClicked);
    connect(m_editButton, &QPushButton::clicked, this, &UsersView::onEditButtonClicked);

    connect(&m_viewModel, &UsersManagementViewModel::usersChanged, this,
            &UsersView::onUsersChanged);

    m_viewModel.fetchUsers();

    // We can't add new users this way for now
    // TODO: https://sageteam.atlassian.net/browse/SS-32
    m_addButton->setHidden(true);
}

void UsersView::onDeleteButtonClicked()
{
    SPDLOG_TRACE("UsersView::onDeleteButtonClicked");
    auto selectedItems = m_dataTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        int row        = selectedItems.first()->row();
        QString userId = m_dataTable->item(row, 0)->text(); // Get user ID from the hidden column
        m_viewModel.deleteUser(userId);
    } else {
        SPDLOG_WARN("No user selected for deletion.");
    }
}

void UsersView::onEditButtonClicked()
{
    SPDLOG_TRACE("UsersView::onEditButtonClicked");

    // Clicking on btn it's not very good-looking solution
    // TODO: https://sageteam.atlassian.net/browse/SS-31

    auto selectedItems = m_dataTable->selectedItems();
    if (!selectedItems.isEmpty()) {
        int row = selectedItems.first()->row();
        DisplayData::User user;
        user.id       = m_dataTable->item(row, 0)->text();
        user.username = m_dataTable->item(row, 1)->text();
        user.password = m_dataTable->item(row, 2)->text();
        user.roleId   = m_dataTable->item(row, 3)->text();
        user.roleName = m_dataTable->item(row, 4)->text();
        m_viewModel.editUser(user);
    } else {
        SPDLOG_WARN("No user selected for deletion.");
    }
}

void UsersView::onUsersChanged()
{
    SPDLOG_TRACE("UsersView::onUsersChanged");
    auto users = m_viewModel.users();
    fillTable(users);
}

void UsersView::fillTable(const QVector<DisplayData::User> &users)
{
    SPDLOG_TRACE("UsersView::fillTable");
    m_dataTable->clear();
    m_dataTable->setRowCount(users.size());
    m_dataTable->setColumnCount(DisplayData::User::VAR_COUNT);
    QStringList headers = {"ID", "Username", "Password", "Role ID", "Role Name"};
    m_dataTable->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < users.size(); ++row) {
        const auto &user = users[row];
        m_dataTable->setItem(row, 0, new QTableWidgetItem(user.id));
        m_dataTable->setItem(row, 1, new QTableWidgetItem(user.username));
        m_dataTable->setItem(row, 2, new QTableWidgetItem(user.password));
        m_dataTable->setItem(row, 3, new QTableWidgetItem(user.roleId));
        m_dataTable->setItem(row, 4, new QTableWidgetItem(user.roleName));
    }

    // Hide the ID and Role ID columns
    m_dataTable->setColumnHidden(0, true);
    m_dataTable->setColumnHidden(3, true);
}
