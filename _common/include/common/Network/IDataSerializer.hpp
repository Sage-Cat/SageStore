#pragma once

#include "common/DataTypes.hpp"

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
    virtual ~IDataSerializer() = default;

    virtual std::string serialize(const Dataset &dataset) = 0;
    virtual Dataset deserialize(const std::string &serializedData) = 0;
};
