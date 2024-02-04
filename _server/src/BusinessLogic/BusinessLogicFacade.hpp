#pragma once

#include <string>

#include "DataSpecs.hpp"

class RequestData;

class BusinessLogicFacade
{
public:
    std::string executeTask(const RequestData &requestData, BusinessLogicCallback callback);

private:


};
