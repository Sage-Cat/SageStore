#include <QSignalSpy>
#include <QtTest>

#include "Ui/Models/PurchaseOrdersModel.hpp"

#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"

#include "mocks/ApiManagerMock.hpp"

class PurchaseOrdersModelTest : public QObject {
    Q_OBJECT

    PurchaseOrdersModel *purchaseOrdersModel;
    ApiManagerMock *apiManagerMock;

public:
    PurchaseOrdersModelTest()
    {
        apiManagerMock       = new ApiManagerMock();
        purchaseOrdersModel  = new PurchaseOrdersModel(*apiManagerMock);
    }

    ~PurchaseOrdersModelTest() { delete purchaseOrdersModel; }

private slots:
    void testFetchOrders()
    {
        QSignalSpy ordersChangedSpy(purchaseOrdersModel, &PurchaseOrdersModel::ordersChanged);
        purchaseOrdersModel->fetchOrders();
        QCOMPARE(usersChangedSpy.count(), 1);
    }

    void testAddOrder()
    {
        QSignalSpy orderAddedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderAdded);
        purchaseOrdersModel->addOrder(Common::Entities::PurchaseOrder{
            .id = "1", .date = "2021-01-01", .userId = "1", .supplierId = "1" .status = "newStatus"});
        QCOMPARE(orderAddedSpy.count(), 1);
    }

    void testEditOrder()
    {
        QSignalSpy orderEditedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderEdited);
        purchaseOrdersModel->addOrder(Common::Entities::PurchaseOrder{
            .id = "1", .date = "2021-02-01", .userId = "2", .supplierId = "2" .status = "updateStatus"});
        QCOMPARE(orderAddedSpy.count(), 1);
    }

    void testDeleteOrder()
    {
        QSignalSpy orderDeletedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderDeleted);
        purchaseOrdersModel->deleteOrder("1");
        QCOMPARE(orderDeletedSpy.count(), 1);
    }

    void testFetchOrderRecords()
    {
        QSignalSpy orderRecordsChangedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderRecordsChanged);
        purchaseOrdersModel->fetchOrderRecords();
        QCOMPARE(orderRecordsChangedSpy.count(), 1);
    }

    void testCreateOrderRecord()
    {
        QSignalSpy orderRecordCreatedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderRecordCreated);
        purchaseOrdersModel->createOrderRecord(Common::Entities::PurchaseOrderRecord{
            .id = "1", .orderId = "1", .productTypeId = "1", .quantity = "1", .price = "1.0"});
        QCOMPARE(orderRecordCreatedSpy.count(), 1);

    }

    void editOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord)
    {
        QSignalSpy orderRecordEditedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderRecordEdited);
        purchaseOrdersModel->editOrderRecord(Common::Entities::PurchaseOrderRecord{
            .id = "1", .orderId = "2", .productTypeId = "2", .quantity = "2", .price = "5.0"});
        QCOMPARE(orderRecordEditedSpy.count(), 1);
    }

    void deleteOrderRecord(const QString &id)
    {
        QSignalSpy orderRecordDeletedSpy(purchaseOrdersModel, &PurchaseOrdersModel::orderRecordDeleted);
        purchaseOrdersModel->deleteOrderRecord("1");
        QCOMPARE(orderRecordDeletedSpy.count(), 1);
    }
};