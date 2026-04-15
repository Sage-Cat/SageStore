#pragma once

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

class SystemModule : public IBusinessModule {
public:
    ResponseData executeTask(const RequestData &requestData) override;
};
