#pragma once

#include <QJsonObject>
#include <QString>

namespace DebugCli {

struct OperationResult {
    bool ok{false};
    QString category{QStringLiteral("infra")};
    QString message;
    QJsonObject result;

    static OperationResult success(QJsonObject details = {}, const QString &message = {})
    {
        return OperationResult{
            .ok = true, .category = {}, .message = message, .result = std::move(details)};
    }

    static OperationResult failure(const QString &category, const QString &message,
                                   QJsonObject details = {})
    {
        return OperationResult{
            .ok = false, .category = category, .message = message, .result = std::move(details)};
    }
};

} // namespace DebugCli
