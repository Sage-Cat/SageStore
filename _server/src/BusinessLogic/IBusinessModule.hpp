/**
 * @file IBusinessModule.hpp
 * @brief Header file for the IBusinessModule interface.
 */

#ifndef IBUSINESSMODULE_HPP
#define IBUSINESSMODULE_HPP

/**
 * @interface IBusinessModule
 * @brief Interface for business modules.
 *
 * This interface defines the basic functionalities that any business module should implement.
 */
class IBusinessModule
{
public:
    /**
     * @brief Destructor for the IBusinessModule class.
     */
    virtual ~IBusinessModule() = default;

    /**
     * @brief Initialize the business module.
     *
     * This function is used to initialize the business module and prepare it for operation.
     */
    virtual void init() = 0;
};

#endif // IBUSINESSMODULE_HPP
