#pragma once

#include <map>
#include <vector>

#include <QObject>

#include "common/DataTypes.hpp"
#include "common/Entities/Role.hpp"
#include "common/Entities/User.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"

class NetworkService;
enum class Method;

class ApiManager : public QObject {
    Q_OBJECT

    using ResponseHandler = std::function<void(Method method, const Dataset &)>;

public:
    ApiManager(NetworkService &networkService);
    ~ApiManager();

public slots:
    // Users
    virtual void loginUser(const QString &username, const QString &password);
    virtual void getUsers();
    virtual void addUser(const Common::Entities::User &user);
    virtual void editUser(const Common::Entities::User &user);
    virtual void deleteUser(const QString &id);

    // Roles
    virtual void getRoleList();
    virtual void createRole(const Common::Entities::Role &role);
    virtual void editRole(const Common::Entities::Role &role);
    virtual void deleteRole(const QString &id);

    // Purchase Orders and Purchase Order Records aren't implemented and connected in the snippet. It's just a plug.

    // Purchase Orders
    virtual void getPurchaseOrders();
    virtual void createPurchaseOrder(const Common::Entities::PurchaseOrder &purchaseOrder);
    virtual void editPurchaseOrder(const Common::Entities::PurchaseOrder &purchaseOrder);
    virtual void deletePurchaseOrder(const QString &id);

    // Purchase Order Records
    virtual void getPurchaseOrderRecords();
    virtual void createPurchaseOrderRecord(const Common::Entities::PurchaseOrder &purchaseOrder);
    virtual void editPurchaseOrderRecord(const Common::Entities::PurchaseOrder &purchaseOrder);
    virtual void deletePurchaseOrderRecord(const QString &id);

protected slots:
    // for NetworkService
    virtual void handleResponse(const std::string &endpoint, Method method, const Dataset &dataset);

signals:
    // Startup
    void ready();

    // Users
    void loginSuccess(const QString &id, const QString &roleId);
    void userAdded();
    void userEdited();
    void userDeleted();
    void usersList(const std::vector<Common::Entities::User> &users);

    // Roles
    void rolesList(const std::vector<Common::Entities::Role> &roleList);
    void roleCreated();
    void roleEdited();
    void roleDeleted();

    // Purchase Orders and Purchase Order Records aren't implemented and connected in the snippet. It's just a plug.

    // Purchase Orders
    void purchaseOrdersList(const std::vector<Common::Entities::PurchaseOrder> &purchaseOrders);
    void purchaseOrderAdded();
    void purchaseOrderEdited();
    void purchaseOrderDeleted();

    // Purchase Order Records
    void purchaseOrderRecordsList(const std::vector<Common::Entities::PurchaseOrderRecord> &purchaseOrderRecords);
    void purchaseOrderRecordCreated();
    void purchaseOrderRecordEdited();
    void purchaseOrderRecordDeleted();


    // Error handling
    void errorOccurred(const QString &errorMessage);

private:
    void init();
    void setupHandlers();

    // handlers
    void handleError(const std::string &errorMessage);
    void handleLoginResponse(Method method, const Dataset &dataset);
    void handleUsers(Method method, const Dataset &dataset);
    void handleRoles(Method method, const Dataset &dataset);

private:
    void handleRolesList(const Dataset &dataset);
    void handleUsersList(const Dataset &dataset);

private:
    NetworkService &m_networkService;
    std::map<std::string, ResponseHandler> m_responseHandlers;
};
