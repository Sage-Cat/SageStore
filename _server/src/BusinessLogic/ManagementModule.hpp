#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Supplier.hpp"

class RepositoryManager;

class ManagementModule : public IBusinessModule {
public:
    explicit ManagementModule(RepositoryManager &repositoryManager);
    ~ManagementModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getContacts() const;
    ResponseData getSuppliers() const;
    ResponseData getEmployees() const;
    void addContact(const Dataset &request) const;
    void editContact(const Dataset &request, const std::string &resourceId) const;
    void deleteContact(const std::string &resourceId) const;
    void addSupplier(const Dataset &request) const;
    void editSupplier(const Dataset &request, const std::string &resourceId) const;
    void deleteSupplier(const std::string &resourceId) const;
    void addEmployee(const Dataset &request) const;
    void editEmployee(const Dataset &request, const std::string &resourceId) const;
    void deleteEmployee(const std::string &resourceId) const;

    std::shared_ptr<IRepository<Common::Entities::Contact>> m_contactsRepository;
    std::shared_ptr<IRepository<Common::Entities::Supplier>> m_suppliersRepository;
    std::shared_ptr<IRepository<Common::Entities::Employee>> m_employeesRepository;
};
