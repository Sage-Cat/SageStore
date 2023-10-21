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

    /**
     * @brief Initializes the model.
     *
     * This pure virtual function should be implemented by derived classes to handle their initialization logic.
     */
    virtual void init() = 0;
};
