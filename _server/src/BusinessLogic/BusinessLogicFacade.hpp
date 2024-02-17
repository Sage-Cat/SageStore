#pragma once

#include <string>

#include "DataSpecs.hpp"

class BusinessLogicFacade
{
public:
    std::string executeTask(RequestData requestData, BusinessLogicCallback callback);

private:


};
