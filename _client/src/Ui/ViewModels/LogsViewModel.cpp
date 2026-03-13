#include "Ui/ViewModels/LogsViewModel.hpp"

#include "Ui/Models/LogsModel.hpp"

LogsViewModel::LogsViewModel(LogsModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &LogsModel::logsChanged, this, &LogsViewModel::logsChanged);
    connect(&m_model, &LogsModel::errorOccurred, this, &LogsViewModel::errorOccurred);
}

QVector<Common::Entities::Log> LogsViewModel::logs() const { return m_model.logs(); }

QString LogsViewModel::userLabel(const QString &userId) const
{
    if (userId.isEmpty()) {
        return tr("System");
    }

    for (const auto &user : m_model.users()) {
        if (QString::fromStdString(user.id) == userId) {
            return QString::fromStdString(user.username);
        }
    }
    return userId;
}

void LogsViewModel::fetchAll() { m_model.fetchAll(); }
