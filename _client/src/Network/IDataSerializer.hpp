#pragma once

#include <QByteArray>

#include "DataTypes.hpp"

/**
 * @interface IDataSerializer
 * @brief Interface for data serialization and deserialization.
 */
class IDataSerializer
{
public:
    virtual ~IDataSerializer() = default;

    virtual QByteArray serialize(const Dataset &dataset) = 0;
    virtual Dataset deserialize(const QByteArray &serializedData) = 0;
};
