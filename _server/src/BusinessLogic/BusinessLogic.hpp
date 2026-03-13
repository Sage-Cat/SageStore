#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "DataSpecs.hpp"
#include "Database/IRepository.hpp"

#include "common/Entities/Log.hpp"

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
    std::shared_ptr<IRepository<Common::Entities::Log>> m_logRepository;
};
