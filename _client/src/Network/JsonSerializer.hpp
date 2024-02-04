#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QStringList>

#include "IDataSerializer.hpp"

/**
 * @class JsonSerializer
 * @brief Provides methods for serializing and deserializing data to and from JSON format using Qt's JSON capabilities.
 *
 * Inherits from IDataSerializer interface to implement serialization and deserialization of data sets to and from JSON.
 */
class JsonSerializer : public IDataSerializer
{
public:
    /**
     * @brief Serializes a Dataset object into a QByteArray in JSON format.
     *
     * @param dataset The Dataset object to be serialized.
     * @return QByteArray containing the serialized JSON data.
     */
    QByteArray serialize(const Dataset &dataset) override;

    /**
     * @brief Deserializes a QByteArray containing JSON data back into a Dataset object.
     *
     * @param serializedData The QByteArray containing the JSON data to be deserialized.
     * @return Dataset object generated from the serialized JSON data.
     */
    Dataset deserialize(const QByteArray &serializedData) override;
};
