#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "DataSpecs.hpp"

class RepositoryManager;
class IBusinessModule;

class BusinessLogic {
public:
    BusinessLogic(RepositoryManager &repositoryManager);
    ~BusinessLogic();

    void executeTask(RequestData requestData, BusinessLogicCallback callback);

private:
    using ModuleRegistry = std::unordered_map<std::string, std::unique_ptr<IBusinessModule>>;

    ModuleRegistry m_modules;
};
