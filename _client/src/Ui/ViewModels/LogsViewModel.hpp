#pragma once

#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/Log.hpp"
#include "common/Entities/User.hpp"

class LogsModel;

class LogsViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit LogsViewModel(LogsModel &model, QObject *parent = nullptr);

    QVector<Common::Entities::Log> logs() const;
    QString userLabel(const QString &userId) const;

signals:
    void logsChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();

private:
    LogsModel &m_model;
};
