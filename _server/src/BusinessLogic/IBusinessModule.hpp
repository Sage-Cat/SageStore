#pragma once

#include "DataSpecs.hpp"

/**
 * @interface IBusinessModule
 * @brief Interface for business modules.
 */
class IBusinessModule
{
public:
    virtual ~IBusinessModule() = default;
    virtual ResponseData executeTask(const RequestData &requestData) = 0;
};