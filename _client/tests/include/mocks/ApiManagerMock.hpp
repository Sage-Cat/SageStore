#pragma once

#include "Network/ApiManager.hpp"

#include "mocks/NetworkServiceMock.hpp"

class ApiManagerMock : public ApiManager {
public:
    ApiManagerMock() : ApiManager(*(new NetworkServiceMock)) {}

public slots:
    // Users
    void addUser(const Common::Entities::User &user) override { emit userAdded(); }
    void editUser(const Common::Entities::User &user) override { emit userEdited(); }
    void deleteUser(const QString &id) override { emit userDeleted(); }
    void getUsers() override { emit usersList({}); }
    
    // Roles
    void createRole(const Common::Entities::Role &role) override { emit roleCreated(); }
    void editRole(const Common::Entities::Role &role) override { emit roleEdited(); }
    void deleteRole(const QString &id) override { emit roleDeleted(); }
    void getRoleList() override { emit rolesList({}); }
    
    // Purchase Orders
    void createPurchaseOrder(const Common::Entities::PurchaseOrder &purchaseOrder) override { emit purchaseOrderAdded(); }
    void editPurchaseOrder(const Common::Entities::PurchaseOrder &purchaseOrder) override { emit purchaseOrderEdited(); }
    void deletePurchaseOrder(const QString &id) override { emit purchaseOrderDeleted(); }
    void getPurchaseOrders() override { emit purchaseOrdersList({}); }

    // Purchase Order Records
    void createPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord) override { emit purchaseOrderRecordCreated(); }
    void editPurchaseOrderRecord(const Common::Entities::PurchaseOrderRecord &purchaseOrderRecord) override { emit purchaseOrderRecordEdited(); }
    void deletePurchaseOrderRecord(const QString &id) override { emit purchaseOrderRecordDeleted(); }
    void getPurchaseOrderRecords() override { emit purchaseOrderRecordsList({}); }

    // Error handling
    void emitError(const QString &errorMessage) { emit errorOccurred(errorMessage); }
};
