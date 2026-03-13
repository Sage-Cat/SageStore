#pragma once

#include <memory>

#include "DataSpecs.hpp"
#include "IBusinessModule.hpp"

#include "Database/IRepository.hpp"

#include "common/Entities/Contact.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/User.hpp"

class RepositoryManager;

class SalesModule : public IBusinessModule {
public:
    explicit SalesModule(RepositoryManager &repositoryManager);
    ~SalesModule() override;

    ResponseData executeTask(const RequestData &requestData) override;

private:
    ResponseData getOrders() const;
    ResponseData getOrderRecords(const std::string &orderId) const;
    void addOrder(const Dataset &request) const;
    void editOrder(const Dataset &request, const std::string &resourceId) const;
    void deleteOrder(const std::string &resourceId) const;
    void addOrderRecord(const Dataset &request) const;
    void editOrderRecord(const Dataset &request, const std::string &resourceId) const;
    void deleteOrderRecord(const std::string &resourceId) const;

    std::shared_ptr<IRepository<Common::Entities::SaleOrder>> m_ordersRepository;
    std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>> m_orderRecordsRepository;
    std::shared_ptr<IRepository<Common::Entities::User>> m_usersRepository;
    std::shared_ptr<IRepository<Common::Entities::Contact>> m_contactsRepository;
    std::shared_ptr<IRepository<Common::Entities::Employee>> m_employeesRepository;
    std::shared_ptr<IRepository<Common::Entities::ProductType>> m_productTypesRepository;
};
