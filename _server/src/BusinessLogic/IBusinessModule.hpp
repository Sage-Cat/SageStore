#pragma once

#include "DataSpecs.hpp"

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
     * @brief Executes action according to requested action and requestId (if specified)
     */
    virtual ResponseData executeTask(const RequestData &requestData) = 0;
};