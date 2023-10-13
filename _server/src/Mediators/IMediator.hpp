#pragma once

/**
 * @interface IMediator
 * @brief Interface for mediator pattern.
 *
 * This interface defines the basic functionality for mediating communication
 * between different components or objects.
 */
class IMediator
{
public:
    /**
     * @brief Destructor for the IMediator class.
     */
    virtual ~IMediator() = default;

    /**
     * @brief Mediate a communication between components or objects.
     *
     * This function is used to mediate communication between different components
     * or objects, decoupling them from direct interaction with each other.
     */
    virtual void mediate() = 0;
};
