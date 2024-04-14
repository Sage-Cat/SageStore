#pragma once

#include "Network/IDataSerializer.hpp"

/**
 * @class JsonSerializer
 * @brief Implements IDataSerializer to provide JSON serialization and deserialization for Dataset objects.
 */
class JsonSerializer : public IDataSerializer
{
public:
    std::string serialize(const Dataset &dataset) override;
    Dataset deserialize(const std::string &serializedData) override;
};
