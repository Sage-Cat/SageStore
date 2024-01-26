#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QStringList>

#include "IDataSerializer.hpp"

/**
 * @brief The JsonSerializer class
 * Provides methods for serializing and deserializing data to and from JSON format.
 * It makes use of Qt's JSON handling capabilities.
 */
class JsonSerializer : public IDataSerializer
{
public:
    QByteArray serialize(const Dataset &dataset) override;
    Dataset deserialize(const QByteArray &serializedData) override;
};
