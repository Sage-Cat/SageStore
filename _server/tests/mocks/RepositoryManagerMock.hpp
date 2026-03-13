#include <gmock/gmock.h>

#include <memory>

#include "Database/RepositoryManager.hpp"
#include "DatabaseManagerMock.hpp"

#include "common/Entities/Role.hpp"
#include "common/Entities/Contact.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Entities/User.hpp"

class SqliteDatabaseManager;

class RepositoryManagerMock : public RepositoryManager {
public:
    RepositoryManagerMock()
        : RepositoryManager(std::make_shared<::testing::NiceMock<DatabaseManagerMock>>())
    {
    }

    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::User>>, getUserRepository, (),
                (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Role>>, getRoleRepository, (),
                (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Inventory>>,
                getInventoryRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Contact>>, getContactRepository, (),
                (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Employee>>,
                getEmployeeRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Log>>, getLogRepository, (),
                (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::PurchaseOrder>>,
                getPurchaseOrderRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::PurchaseOrderRecord>>,
                getPurchaseOrderRecordRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::ProductType>>,
                getProductTypeRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::SaleOrder>>,
                getSaleOrderRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::SalesOrderRecord>>,
                getSalesOrderRecordRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::Supplier>>,
                getSupplierRepository, (), (override));
    MOCK_METHOD(std::shared_ptr<IRepository<Common::Entities::SuppliersProductInfo>>,
                getSuppliersProductInfoRepository, (), (override));
};
