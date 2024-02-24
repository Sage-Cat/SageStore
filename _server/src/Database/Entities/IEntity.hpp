#pragma once

#include "DatasetCommon.hpp"
#include "DataSpecs.hpp"

class IEntity
{
    virtual Dataset &operator>>(Dataset &dataset) = 0;
};