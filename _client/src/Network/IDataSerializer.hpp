#pragma once

#include <QByteArray>

#include "DataTypes.hpp"

/**
 * @interface IDataSerializer
 * @brief Interface for data serialization and deserialization.
 *
 * Defines basic functionalities for serializing and deserializing data.
 * This interface can be implemented for different data formats like JSON, XML, etc.
 */
class IDataSerializer
{
public:
    /**
     * @brief Virtual destructor for IDataSerializer.
     * Ensures proper cleanup for derived classes.
     */
    virtual ~IDataSerializer() = default;

    /**
     * @brief Serializes 2-dimensional data into a QByteArray.
     * @param data2D The 2-dimensional data to serialize.
     * @return QByteArray containing the serialized data.
     */
    virtual QByteArray serialize(const Dataset &dataset) = 0;

    /**
     * @brief Deserializes data from a QByteArray into a 2-dimensional format.
     * @param serializedData QByteArray containing the data to be deserialized.
     * @return The deserialized 2-dimensional data.
     */
    virtual Dataset deserialize(const QByteArray &serializedData) = 0;
};
