#pragma once

/**
 * @interface IModule
 * @brief Interface for modules.
 *
 * This interface defines the basic functionalities that any module should provide.
 */
class IModule
{
public:
    /**
     * @brief Virtual destructor for IModule.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~IModule() = default;

    /**
     * @brief Initializes the module.
     *
     * This pure virtual function should be implemented by derived classes to handle their initialization logic.
     */
    virtual void init() = 0;
};
