#pragma once

#include <QObject>
#include <QVector>

#include "Ui/DisplayData/Entities.hpp"
#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/PurchaseOrder.hpp"

class PurchaseOrdersModel;
class UsersManagementModel;
class ManagementModel;

class PurchaseOrdersViewModel : public BaseViewModel {
    Q_OBJECT
public:
    explicit PurchaseOrdersViewModel(PurchaseOrdersModel &model, UsersManagementModel &usr_model, ManagementModel &man_model, QObject *parent = nullptr);

    QVector<DisplayData::PurchaseOrder> orders() const;
    QVector<DisplayData::User> users() const;
    QVector<DisplayData::Supplier> suppliers() const;

signals:
    void ordersChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchOrders();
    void addOrder(const DisplayData::PurchaseOrder &order);
    void editOrder(const DisplayData::PurchaseOrder &order);
    void deleteOrder(const QString &id);

private slots:
    void onOrdersChanged();

private:
    DisplayData::PurchaseOrder convertToDisplayOrder(const Common::Entities::PurchaseOrder &commonOrder) const;
    Common::Entities::PurchaseOrder convertToCommonOrder(const DisplayData::PurchaseOrder &displayOrder) const;

private:
    PurchaseOrdersModel &m_model;
    QVector<DisplayData::PurchaseOrder> m_orders;

    std::function<QString(const std::string)> getUserById;
    std::function<QString(const std::string)> getSupplierById;
};
