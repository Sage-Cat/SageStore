#include <QSignalSpy>
#include <QtTest>

#include "Network/ApiManager.hpp"

#include "common/AnalyticsKeys.hpp"

#include "mocks/NetworkServiceMock.hpp"

class ApiManagerTest : public QObject {
    Q_OBJECT

    ApiManager *apiManager;
    NetworkServiceMock *networkServiceMock;

public:
    ApiManagerTest()
    {
        networkServiceMock = new NetworkServiceMock;
        apiManager         = new ApiManager(*networkServiceMock);
    }

    ~ApiManagerTest() { delete apiManager; }

private slots:
    void testSuccessfulLogin()
    {
        QSignalSpy loginSuccessSpy(apiManager, &ApiManager::loginSuccess);
        apiManager->loginUser("testUser", "testPassword");
        QCOMPARE(loginSuccessSpy.count(), 1);
    }

    void testSuccessfulGetRole()
    {
        QSignalSpy getSuccessSpy(apiManager, &ApiManager::rolesList);
        apiManager->getRoleList();
        QCOMPARE(getSuccessSpy.count(), 1);
    }

    void testSuccessfulAddRole()
    {
        QSignalSpy addSuccessSpy(apiManager, &ApiManager::roleCreated);
        apiManager->createRole(Common::Entities::Role{.name = "testName"});
        QCOMPARE(addSuccessSpy.count(), 1);
    }

    void testSuccessfulEditRoles()
    {
        QSignalSpy editSuccessSpy(apiManager, &ApiManager::roleEdited);
        apiManager->editRole(Common::Entities::Role{.id = "0", .name = "testName"});
        QCOMPARE(editSuccessSpy.count(), 1);
    }

    void testSuccessfulDeleteRole()
    {
        QSignalSpy deleteSuccessSpy(apiManager, &ApiManager::roleDeleted);
        apiManager->deleteRole("0");
        QCOMPARE(deleteSuccessSpy.count(), 1);
    }

    void testSuccessfulGetUsers()
    {
        QSignalSpy getUsersSuccessSpy(apiManager, &ApiManager::usersList);
        apiManager->getUsers();
        QCOMPARE(getUsersSuccessSpy.count(), 1);
    }

    void testSuccessfulAddUser()
    {
        QSignalSpy addUserSuccessSpy(apiManager, &ApiManager::userAdded);
        apiManager->addUser(Common::Entities::User{
            .id = "1", .username = "newUser", .password = "newPassword", .roleId = "1"});
        QCOMPARE(addUserSuccessSpy.count(), 1);
    }

    void testSuccessfulUpdateUser()
    {
        QSignalSpy updateUserSuccessSpy(apiManager, &ApiManager::userEdited);
        apiManager->editUser(Common::Entities::User{
            .id = "1", .username = "updatedUser", .password = "updatedPassword", .roleId = "1"});
        QCOMPARE(updateUserSuccessSpy.count(), 1);
    }

    void testSuccessfulDeleteUser()
    {
        QSignalSpy deleteUserSuccessSpy(apiManager, &ApiManager::userDeleted);
        apiManager->deleteUser("1");
        QCOMPARE(deleteUserSuccessSpy.count(), 1);
    }

    void testSuccessfulGetProductTypes()
    {
        QSignalSpy getProductTypesSpy(apiManager, &ApiManager::productTypesList);
        apiManager->getProductTypes();
        QCOMPARE(getProductTypesSpy.count(), 1);
    }

    void testSuccessfulCreateProductType()
    {
        QSignalSpy createdSpy(apiManager, &ApiManager::productTypeCreated);
        apiManager->createProductType(Common::Entities::ProductType{
            .id = "",
            .code = "PT-001",
            .barcode = "123",
            .name = "Oil",
            .description = "Synthetic oil",
            .lastPrice = "10.50",
            .unit = "pcs",
            .isImported = "0"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testSuccessfulEditProductType()
    {
        QSignalSpy editedSpy(apiManager, &ApiManager::productTypeEdited);
        apiManager->editProductType(Common::Entities::ProductType{
            .id = "1",
            .code = "PT-001",
            .barcode = "123",
            .name = "Oil updated",
            .description = "Synthetic oil updated",
            .lastPrice = "11.00",
            .unit = "pcs",
            .isImported = "1"});
        QCOMPARE(editedSpy.count(), 1);
    }

    void testSuccessfulDeleteProductType()
    {
        QSignalSpy deletedSpy(apiManager, &ApiManager::productTypeDeleted);
        apiManager->deleteProductType("1");
        QCOMPARE(deletedSpy.count(), 1);
    }

    void testSuccessfulGetStocks()
    {
        QSignalSpy getStocksSpy(apiManager, &ApiManager::stocksList);
        apiManager->getStocks();
        QCOMPARE(getStocksSpy.count(), 1);
    }

    void testSuccessfulCreateStock()
    {
        QSignalSpy createdSpy(apiManager, &ApiManager::stockCreated);
        apiManager->createStock(Common::Entities::Inventory{
            .id = "", .productTypeId = "1", .quantityAvailable = "14", .employeeId = "1"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testSuccessfulEditStock()
    {
        QSignalSpy editedSpy(apiManager, &ApiManager::stockEdited);
        apiManager->editStock(Common::Entities::Inventory{
            .id = "1", .productTypeId = "1", .quantityAvailable = "17", .employeeId = "2"});
        QCOMPARE(editedSpy.count(), 1);
    }

    void testSuccessfulDeleteStock()
    {
        QSignalSpy deletedSpy(apiManager, &ApiManager::stockDeleted);
        apiManager->deleteStock("1");
        QCOMPARE(deletedSpy.count(), 1);
    }

    void testSuccessfulGetContacts()
    {
        QSignalSpy getContactsSpy(apiManager, &ApiManager::contactsList);
        apiManager->getContacts();
        QCOMPARE(getContactsSpy.count(), 1);
    }

    void testSuccessfulCreateContact()
    {
        QSignalSpy createdSpy(apiManager, &ApiManager::contactCreated);
        apiManager->createContact(Common::Entities::Contact{
            .id = "",
            .name = "New Customer",
            .type = "Customer",
            .email = "customer@example.com",
            .phone = "123456789"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testSuccessfulGetSupplierProducts()
    {
        QSignalSpy getSupplierProductsSpy(apiManager, &ApiManager::supplierProductsList);
        apiManager->getSupplierProducts();
        QCOMPARE(getSupplierProductsSpy.count(), 1);
    }

    void testSuccessfulCreateSupplierProduct()
    {
        QSignalSpy createdSpy(apiManager, &ApiManager::supplierProductCreated);
        apiManager->createSupplierProduct(Common::Entities::SuppliersProductInfo{
            .id = "", .supplierID = "1", .productTypeId = "1", .code = "SUP-NEW"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testSuccessfulGetPurchaseOrders()
    {
        QSignalSpy getPurchaseOrdersSpy(apiManager, &ApiManager::purchaseOrdersList);
        apiManager->getPurchaseOrders();
        QCOMPARE(getPurchaseOrdersSpy.count(), 1);
    }

    void testSuccessfulCreatePurchaseOrder()
    {
        QSignalSpy createdSpy(apiManager, &ApiManager::purchaseOrderCreated);
        apiManager->createPurchaseOrder(Common::Entities::PurchaseOrder{
            .id = "",
            .date = "2026-03-13",
            .userId = "1",
            .supplierId = "1",
            .status = "Ordered"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testSuccessfulGetPurchaseOrderRecords()
    {
        QSignalSpy getPurchaseOrderRecordsSpy(apiManager, &ApiManager::purchaseOrderRecordsList);
        apiManager->getPurchaseOrderRecords("1");
        QCOMPARE(getPurchaseOrderRecordsSpy.count(), 1);
    }

    void testSuccessfulPostPurchaseReceipt()
    {
        QSignalSpy postedSpy(apiManager, &ApiManager::purchaseReceiptPosted);
        apiManager->postPurchaseReceipt("1", "1");
        QCOMPARE(postedSpy.count(), 1);
    }

    void testSuccessfulGetSalesOrders()
    {
        QSignalSpy getSalesOrdersSpy(apiManager, &ApiManager::salesOrdersList);
        apiManager->getSalesOrders();
        QCOMPARE(getSalesOrdersSpy.count(), 1);
    }

    void testSuccessfulCreateSalesOrder()
    {
        QSignalSpy createdSpy(apiManager, &ApiManager::salesOrderCreated);
        apiManager->createSalesOrder(Common::Entities::SaleOrder{
            .id = "",
            .date = "2026-03-13",
            .userId = "1",
            .contactId = "1",
            .employeeId = "1",
            .status = "Confirmed"});
        QCOMPARE(createdSpy.count(), 1);
    }

    void testSuccessfulGetSalesOrderRecords()
    {
        QSignalSpy getSalesOrderRecordsSpy(apiManager, &ApiManager::salesOrderRecordsList);
        apiManager->getSalesOrderRecords("1");
        QCOMPARE(getSalesOrderRecordsSpy.count(), 1);
    }

    void testSuccessfulGetLogs()
    {
        QSignalSpy getLogsSpy(apiManager, &ApiManager::logsList);
        apiManager->getLogs();
        QCOMPARE(getLogsSpy.count(), 1);
    }

    void testSuccessfulGetSalesAnalytics()
    {
        QSignalSpy analyticsSpy(apiManager, &ApiManager::salesAnalyticsReady);
        apiManager->getSalesAnalytics();
        QCOMPARE(analyticsSpy.count(), 1);

        const auto payload = analyticsSpy.takeFirst().at(0).toMap();
        QCOMPARE(payload.value(AnalyticsKeys::Sales::TOTAL_ORDERS).toString(), QString("1"));
        QCOMPARE(payload.value(AnalyticsKeys::Sales::TOTAL_REVENUE).toString(),
                 QString("31.50"));
    }

    void testSuccessfulGetInventoryAnalytics()
    {
        QSignalSpy analyticsSpy(apiManager, &ApiManager::inventoryAnalyticsReady);
        apiManager->getInventoryAnalytics();
        QCOMPARE(analyticsSpy.count(), 1);

        const auto payload = analyticsSpy.takeFirst().at(0).toMap();
        QCOMPARE(payload.value(AnalyticsKeys::Inventory::TOTAL_PRODUCT_TYPES).toString(),
                 QString("2"));
        QCOMPARE(payload.value(AnalyticsKeys::Inventory::TOTAL_UNITS_AVAILABLE).toString(),
                 QString("14"));
    }
};

QTEST_MAIN(ApiManagerTest)
#include "ApiManagerTest.moc"
