#pragma once

#include "IDataSerializer.hpp"

class JsonSerializer : public IDataSerializer
{
public:
    std::string serialize(const Dataset &dataset) override;
    Dataset deserialize(const std::string &serializedData) override;
};
