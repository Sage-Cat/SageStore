#include <algorithm>

#include <QSignalSpy>
#include <QtTest>

#include "Ui/Models/AnalyticsModel.hpp"
#include "Ui/Models/LogsModel.hpp"
#include "Ui/Models/ManagementModel.hpp"
#include "Ui/Models/PurchaseModel.hpp"
#include "Ui/Models/SalesModel.hpp"
#include "Ui/Models/SupplierCatalogModel.hpp"

#include "mocks/ApiManagerMock.hpp"

class ErpModelsTest : public QObject {
    Q_OBJECT

private slots:
    void init() { apiManagerMock = new ApiManagerMock(); }

    void cleanup()
    {
        delete apiManagerMock;
        apiManagerMock = nullptr;
    }

    void testManagementModelFetchAndMutationsRefresh()
    {
        ManagementModel model(*apiManagerMock);
        QSignalSpy contactsChangedSpy(&model, &ManagementModel::contactsChanged);
        QSignalSpy suppliersChangedSpy(&model, &ManagementModel::suppliersChanged);
        QSignalSpy employeesChangedSpy(&model, &ManagementModel::employeesChanged);

        model.fetchContacts();
        model.fetchSuppliers();
        model.fetchEmployees();

        QCOMPARE(contactsChangedSpy.count(), 1);
        QCOMPARE(suppliersChangedSpy.count(), 1);
        QCOMPARE(employeesChangedSpy.count(), 1);
        QCOMPARE(model.contacts().size(), 1);
        QCOMPARE(model.suppliers().size(), 1);
        QCOMPARE(model.employees().size(), 1);

        QSignalSpy contactCreatedSpy(&model, &ManagementModel::contactCreated);
        model.createContact(Common::Entities::Contact{
            .id = "",
            .name = "Retail Customer",
            .type = "Customer",
            .email = "retail@example.com",
            .phone = "555-0101"});
        QCOMPARE(contactCreatedSpy.count(), 1);
        QCOMPARE(apiManagerMock->contactsRequestCount(), 2);
        QCOMPARE(model.contacts().size(), 2);

        QSignalSpy supplierEditedSpy(&model, &ManagementModel::supplierEdited);
        model.editSupplier(Common::Entities::Supplier{
            .id = "1",
            .name = "Default Supplier Updated",
            .number = "987654321",
            .email = "supplier@example.com",
            .address = "456 Elm St"});
        QCOMPARE(supplierEditedSpy.count(), 1);
        QCOMPARE(apiManagerMock->suppliersRequestCount(), 2);
        QCOMPARE(model.suppliers().front().name, std::string("Default Supplier Updated"));

        QSignalSpy employeeDeletedSpy(&model, &ManagementModel::employeeDeleted);
        model.deleteEmployee("1");
        QCOMPARE(employeeDeletedSpy.count(), 1);
        QCOMPARE(apiManagerMock->employeesRequestCount(), 2);
        QCOMPARE(model.employees().size(), 0);
    }

    void testSupplierCatalogModelFetchAndMutationsRefresh()
    {
        SupplierCatalogModel model(*apiManagerMock);
        QSignalSpy supplierProductsChangedSpy(&model, &SupplierCatalogModel::supplierProductsChanged);
        QSignalSpy suppliersChangedSpy(&model, &SupplierCatalogModel::suppliersChanged);
        QSignalSpy productTypesChangedSpy(&model, &SupplierCatalogModel::productTypesChanged);

        model.fetchAll();

        QCOMPARE(supplierProductsChangedSpy.count(), 1);
        QCOMPARE(suppliersChangedSpy.count(), 1);
        QCOMPARE(productTypesChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->supplierProductsRequestCount(), 1);
        QCOMPARE(apiManagerMock->suppliersRequestCount(), 1);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 1);
        QCOMPARE(model.supplierProducts().size(), 1);

        QSignalSpy createdSpy(&model, &SupplierCatalogModel::supplierProductCreated);
        model.createSupplierProduct(Common::Entities::SuppliersProductInfo{
            .id = "", .supplierID = "1", .productTypeId = "2", .code = "SUP-002"});
        QCOMPARE(createdSpy.count(), 1);
        QCOMPARE(apiManagerMock->supplierProductsRequestCount(), 2);
        QCOMPARE(model.supplierProducts().size(), 2);

        QSignalSpy editedSpy(&model, &SupplierCatalogModel::supplierProductEdited);
        model.editSupplierProduct(Common::Entities::SuppliersProductInfo{
            .id = "1", .supplierID = "1", .productTypeId = "1", .code = "SUP-001-UPDATED"});
        QCOMPARE(editedSpy.count(), 1);
        QCOMPARE(apiManagerMock->supplierProductsRequestCount(), 3);
        QCOMPARE(model.supplierProducts().front().code, std::string("SUP-001-UPDATED"));

        QSignalSpy deletedSpy(&model, &SupplierCatalogModel::supplierProductDeleted);
        model.deleteSupplierProduct("1");
        QCOMPARE(deletedSpy.count(), 1);
        QCOMPARE(apiManagerMock->supplierProductsRequestCount(), 4);
        QCOMPARE(model.supplierProducts().size(), 1);
    }

    void testPurchaseModelFetchAndReceiptRefresh()
    {
        PurchaseModel model(*apiManagerMock);
        QSignalSpy ordersChangedSpy(&model, &PurchaseModel::ordersChanged);
        QSignalSpy orderRecordsChangedSpy(&model, &PurchaseModel::orderRecordsChanged);
        QSignalSpy receiptPostedSpy(&model, &PurchaseModel::purchaseReceiptPosted);

        model.fetchAll();

        QCOMPARE(ordersChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->purchaseOrdersRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 1);
        QCOMPARE(apiManagerMock->suppliersRequestCount(), 1);
        QCOMPARE(apiManagerMock->employeesRequestCount(), 1);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 1);
        QCOMPARE(model.orders().size(), 1);

        model.fetchOrderRecords("1");
        QCOMPARE(orderRecordsChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->purchaseOrderRecordsRequestCount(), 1);
        QCOMPARE(model.orderRecords().size(), 1);

        model.createOrder(Common::Entities::PurchaseOrder{
            .id = "",
            .date = "2026-03-14",
            .userId = "1",
            .supplierId = "1",
            .status = "Draft"});
        QCOMPARE(apiManagerMock->purchaseOrdersRequestCount(), 2);
        QCOMPARE(apiManagerMock->purchaseOrderRecordsRequestCount(), 2);
        QCOMPARE(model.orders().size(), 2);

        model.createOrderRecord(Common::Entities::PurchaseOrderRecord{
            .id = "", .orderId = "1", .productTypeId = "2", .quantity = "3", .price = "7.50"});
        QCOMPARE(apiManagerMock->purchaseOrderRecordsRequestCount(), 3);
        QCOMPARE(model.orderRecords().size(), 2);

        model.postReceipt("1", "1");
        QCOMPARE(receiptPostedSpy.count(), 1);
        QCOMPARE(apiManagerMock->purchaseOrdersRequestCount(), 3);
        QCOMPARE(apiManagerMock->purchaseOrderRecordsRequestCount(), 4);

        const auto orders = model.orders();
        auto receivedOrderIt = std::find_if(orders.begin(), orders.end(), [](const auto &order) {
            return order.id == "1";
        });
        QVERIFY(receivedOrderIt != orders.end());
        QCOMPARE(receivedOrderIt->status, std::string("Received"));
    }

    void testSalesModelFetchAndMutationsRefresh()
    {
        SalesModel model(*apiManagerMock);
        QSignalSpy ordersChangedSpy(&model, &SalesModel::ordersChanged);
        QSignalSpy orderRecordsChangedSpy(&model, &SalesModel::orderRecordsChanged);

        model.fetchAll();

        QCOMPARE(ordersChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->salesOrdersRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 1);
        QCOMPARE(apiManagerMock->contactsRequestCount(), 1);
        QCOMPARE(apiManagerMock->employeesRequestCount(), 1);
        QCOMPARE(apiManagerMock->productTypesRequestCount(), 1);
        QCOMPARE(model.orders().size(), 1);

        model.fetchOrderRecords("1");
        QCOMPARE(orderRecordsChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->salesOrderRecordsRequestCount(), 1);
        QCOMPARE(model.orderRecords().size(), 1);

        model.createOrder(Common::Entities::SaleOrder{
            .id = "",
            .date = "2026-03-14",
            .userId = "1",
            .contactId = "1",
            .employeeId = "1",
            .status = "Draft"});
        QCOMPARE(apiManagerMock->salesOrdersRequestCount(), 2);
        QCOMPARE(apiManagerMock->salesOrderRecordsRequestCount(), 2);
        QCOMPARE(model.orders().size(), 2);

        model.createOrderRecord(Common::Entities::SalesOrderRecord{
            .id = "", .orderId = "1", .productTypeId = "2", .quantity = "4", .price = "18.20"});
        QCOMPARE(apiManagerMock->salesOrderRecordsRequestCount(), 3);
        QCOMPARE(model.orderRecords().size(), 2);

        model.deleteOrder("1");
        QCOMPARE(apiManagerMock->salesOrdersRequestCount(), 3);
        QCOMPARE(apiManagerMock->salesOrderRecordsRequestCount(), 4);
        QCOMPARE(model.orders().size(), 1);
        QCOMPARE(model.orderRecords().size(), 0);
    }

    void testLogsModelFetchAllAndErrorForwarding()
    {
        LogsModel model(*apiManagerMock);
        QSignalSpy logsChangedSpy(&model, &LogsModel::logsChanged);
        QSignalSpy usersChangedSpy(&model, &LogsModel::usersChanged);
        QSignalSpy errorSpy(&model, &LogsModel::errorOccurred);

        model.fetchAll();

        QCOMPARE(logsChangedSpy.count(), 1);
        QCOMPARE(usersChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->logsRequestCount(), 1);
        QCOMPARE(apiManagerMock->usersRequestCount(), 1);
        QCOMPARE(model.logs().size(), 1);
        QCOMPARE(model.users().size(), 2);

        apiManagerMock->emitError("logs failure");
        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(errorSpy.takeFirst().at(0).toString(), QString("logs failure"));
    }

    void testAnalyticsModelFetchAll()
    {
        AnalyticsModel model(*apiManagerMock);
        QSignalSpy salesMetricsChangedSpy(&model, &AnalyticsModel::salesMetricsChanged);
        QSignalSpy inventoryMetricsChangedSpy(&model, &AnalyticsModel::inventoryMetricsChanged);

        model.fetchAll();

        QCOMPARE(salesMetricsChangedSpy.count(), 1);
        QCOMPARE(inventoryMetricsChangedSpy.count(), 1);
        QCOMPARE(apiManagerMock->salesAnalyticsRequestCount(), 1);
        QCOMPARE(apiManagerMock->inventoryAnalyticsRequestCount(), 1);
        QCOMPARE(model.salesMetrics().value("totalOrders").toString(), QString("1"));
        QCOMPARE(model.inventoryMetrics().value("totalProductTypes").toString(), QString("2"));
    }

private:
    ApiManagerMock *apiManagerMock{nullptr};
};

QTEST_MAIN(ErpModelsTest)
#include "ErpModelsTest.moc"
