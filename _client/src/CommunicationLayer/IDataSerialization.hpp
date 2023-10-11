/**
 * @file IDataSerialization.hpp
 * @brief Header file for the IDataSerialization interface.
 */

#ifndef IDATASERIALIZATION_HPP
#define IDATASERIALIZATION_HPP

#include <QString>

/**
 * @interface IDataSerialization
 * @brief Interface for data serialization and deserialization.
 *
 * This interface defines the basic functionalities for serializing and deserializing data.
 */
class IDataSerialization
{
public:
    /**
     * @brief Virtual destructor for IDataSerialization.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~IDataSerialization() = default;

    /**
     * @brief Serializes the data into a QString.
     *
     * This pure virtual function should be implemented by derived classes to handle their serialization logic.
     * @return A QString containing the serialized data.
     */
    virtual QString serialize() = 0;

    /**
     * @brief Deserializes the data from a QString.
     *
     * This pure virtual function should be implemented by derived classes to handle their deserialization logic.
     * @param data A QString containing the data to be deserialized.
     */
    virtual void deserialize(const QString &data) = 0;
};

#endif // IDATASERIALIZATION_HPP
