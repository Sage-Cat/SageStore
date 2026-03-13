#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QVector>
#include <QWidget>

#include "common/Entities/Log.hpp"

class LogsViewModel;

class LogsView : public QWidget {
    Q_OBJECT

public:
    explicit LogsView(LogsViewModel &viewModel, QWidget *parent = nullptr);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onLogsChanged();
    void onFilterChanged(const QString &text);

private:
    void setupUi();
    void applyFilter();
    void updateStatus(int visibleCount, int totalCount);

    LogsViewModel &m_viewModel;
    QLineEdit *m_filterField{nullptr};
    QTableWidget *m_table{nullptr};
    QLabel *m_statusLabel{nullptr};
    QVector<Common::Entities::Log> m_allLogs;
};
