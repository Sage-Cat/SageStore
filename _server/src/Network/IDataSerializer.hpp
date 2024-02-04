#pragma once

#include "DataSpecs.hpp"

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
     * @brief Serializes Dataset into a QByteArray.
     * @param data2D The Dataset to serialize.
     * @return QByteArray containing the serialized data.
     */
    virtual std::string serialize(const Dataset &dataset) = 0;

    /**
     * @brief Deserializes data from a QByteArray into a Dataset format.
     * @param serializedData QByteArray containing the data to be deserialized.
     * @return The deserialized Dataset data.
     */
    virtual Dataset deserialize(const std::string &serializedData) = 0;
};
