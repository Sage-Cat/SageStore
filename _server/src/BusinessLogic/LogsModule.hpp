#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/Log.hpp"

class RepositoryManager;

class LogsModule : public IBusinessModule {
public:
    explicit LogsModule(RepositoryManager &repositoryManager);
    ~LogsModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getLogs() const;
    void addLog(const Dataset &request) const;
    void editLog(const Dataset &request, const std::string &resourceId) const;
    void deleteLog(const std::string &resourceId) const;

    std::shared_ptr<IRepository<Common::Entities::Log>> m_logsRepository;
};
