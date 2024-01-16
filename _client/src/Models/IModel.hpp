#pragma once

/**
 * @interface IModel
 * @brief Interface for models.
 *
 * This interface defines the basic functionalities that any model should provide.
 */
class IModel
{
public:
    /**
     * @brief Virtual destructor for IModel.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~IModel() = default;
};
