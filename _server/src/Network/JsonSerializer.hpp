#pragma once

#include "IDataSerializer.hpp"

/**
 * @class JsonSerializer
 * @brief Implements IDataSerializer to provide JSON serialization and deserialization for Dataset objects.
 */
class JsonSerializer : public IDataSerializer
{
public:
    /**
     * @brief Serializes a Dataset object to a JSON string.
     *
     * @param dataset The Dataset object to serialize.
     * @return A JSON string representation of the dataset.
     */
    std::string serialize(const Dataset &dataset) override;

    /**
     * @brief Deserializes a JSON string back into a Dataset object.
     *
     * @param serializedData The JSON string to deserialize.
     * @return A Dataset object represented by the serialized data.
     */
    Dataset deserialize(const std::string &serializedData) override;
};
