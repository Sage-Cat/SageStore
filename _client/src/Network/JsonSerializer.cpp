#include "JsonSerializer.hpp"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include "SpdlogWrapper.hpp"

QByteArray JsonSerializer::serialize(const Dataset &dataset)
{
    SPDLOG_TRACE("JsonSerializer::serialize");

    QJsonObject jsonObject;
    for (auto it = dataset.constBegin(); it != dataset.constEnd(); ++it)
    {
        QJsonArray jsonRow;
        for (const QString &cell : it.value())
        {
            jsonRow.append(QJsonValue(cell));
        }
        jsonObject.insert(it.key(), jsonRow);
    }

    QJsonDocument doc(jsonObject);
    return doc.toJson(QJsonDocument::Compact);
}

Dataset JsonSerializer::deserialize(const QByteArray &serializedData)
{
    SPDLOG_TRACE("JsonSerializer::deserialize");

    Dataset dataset;
    QJsonDocument doc = QJsonDocument::fromJson(serializedData);

    if (!doc.isObject())
    {
        SPDLOG_ERROR("Invalid JSON document");
        return dataset;
    }

    QJsonObject jsonObject = doc.object();
    for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
    {
        if (!it.value().isArray())
        {
            SPDLOG_ERROR("Invalid JSON array for key: {}", it.key().toStdString());
            continue;
        }

        QStringList row;
        QJsonArray jsonRow = it.value().toArray();
        for (const QJsonValue &cellValue : jsonRow)
        {
            row.append(cellValue.toString());
        }
        dataset.insert(it.key(), row);
    }

    return dataset;
}
