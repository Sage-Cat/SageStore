#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QStringList>

#include "Network/IDataSerializer.hpp"

/**
 * @class JsonSerializer
 * @brief Provides methods for serializing and deserializing data to and from JSON format using Qt's JSON capabilities.
 */
class JsonSerializer : public IDataSerializer
{
public:
    QByteArray serialize(const Dataset &dataset) override;
    Dataset deserialize(const QByteArray &serializedData) override;
};
