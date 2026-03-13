#include "Ui/Views/LogsView.hpp"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <algorithm>

#include "Ui/ViewModels/LogsViewModel.hpp"

LogsView::LogsView(LogsViewModel &viewModel, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel)
{
    setupUi();

    connect(this, &LogsView::errorOccurred, &m_viewModel, &LogsViewModel::errorOccurred);
    connect(&m_viewModel, &LogsViewModel::logsChanged, this, &LogsView::onLogsChanged);
    connect(m_filterField, &QLineEdit::textChanged, this, &LogsView::onFilterChanged);
}

void LogsView::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    auto *toolbar = new QHBoxLayout;
    auto *titleLabel = new QLabel(tr("Audit Log"), this);
    titleLabel->setObjectName("logsTitleLabel");

    m_filterField = new QLineEdit(this);
    m_filterField->setObjectName("logsFilterField");
    m_filterField->setPlaceholderText(tr("Search by user, action, or timestamp"));
    m_filterField->setClearButtonEnabled(true);

    toolbar->addWidget(titleLabel);
    toolbar->addStretch();
    toolbar->addWidget(m_filterField);

    m_table = new QTableWidget(this);
    m_table->setObjectName("logsTable");
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setSortingEnabled(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_statusLabel = new QLabel(tr("Status: ready"), this);
    m_statusLabel->setObjectName("logsStatusLabel");

    layout->addLayout(toolbar);
    layout->addWidget(m_table);
    layout->addWidget(m_statusLabel);

    setObjectName("logsView");
    setStyleSheet(
        "#logsView { background-color: #fffaf2; }"
        "#logsTitleLabel { color: #233130; font-size: 22px; font-weight: 700; }"
        "QLineEdit { background-color: white; border: 1px solid #cbd5e1; border-radius: 6px; padding: 6px; }"
        "QTableWidget { background-color: #fbfbfd; alternate-background-color: #f1f4f8; border: 1px solid #d7dde7; border-radius: 8px; }"
        "QHeaderView::section { background-color: #e8eef6; color: #0f172a; padding: 6px; border: 0; border-bottom: 1px solid #d7dde7; font-weight: 600; }");
}

void LogsView::onLogsChanged()
{
    m_allLogs = m_viewModel.logs();
    applyFilter();
}

void LogsView::onFilterChanged(const QString &text)
{
    Q_UNUSED(text);
    applyFilter();
}

void LogsView::applyFilter()
{
    const QString filter = m_filterField->text().trimmed();
    QVector<Common::Entities::Log> filtered;
    for (const auto &entry : m_allLogs) {
        const QStringList haystack = {m_viewModel.userLabel(QString::fromStdString(entry.userId)),
                                      QString::fromStdString(entry.action),
                                      QString::fromStdString(entry.timestamp)};
        if (filter.isEmpty() || std::any_of(haystack.begin(), haystack.end(), [&](const QString &value) {
                return value.contains(filter, Qt::CaseInsensitive);
            })) {
            filtered.push_back(entry);
        }
    }

    m_table->setSortingEnabled(false);
    m_table->clearContents();
    m_table->setRowCount(filtered.size());
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({tr("ID"), tr("User"), tr("Action"), tr("Timestamp")});
    for (int row = 0; row < filtered.size(); ++row) {
        const auto &entry = filtered[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(entry.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(
                                     m_viewModel.userLabel(QString::fromStdString(entry.userId))));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(entry.action)));
        m_table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(entry.timestamp)));
    }
    m_table->setColumnHidden(0, true);
    m_table->setSortingEnabled(true);

    updateStatus(filtered.size(), m_allLogs.size());
}

void LogsView::updateStatus(int visibleCount, int totalCount)
{
    m_statusLabel->setText(
        tr("Status: showing %1 of %2 log entries")
            .arg(QString::number(visibleCount), QString::number(totalCount)));
}
