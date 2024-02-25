#pragma once

#include "DataCommon.hpp"
#include "DataSpecs.hpp"

class IEntity
{
    virtual Dataset &operator>>(Dataset &dataset) = 0;
};