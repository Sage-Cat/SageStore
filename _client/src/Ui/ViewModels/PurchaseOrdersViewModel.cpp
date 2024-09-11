#include "Ui/ViewModels/PurchaseOrdersViewModel.hpp"
#include "Ui/Models/PurchaseOrdersModel.hpp"
#include "Ui/Models/UsersManagementModel.hpp"
#include "Ui/Models/ManagementModel.hpp"

#include "common/SpdlogConfig.hpp"

PurchaseOrdersViewModel::PurchaseOrdersViewModel(PurchaseOrdersModel &model, UsersManagementModel &usr_model, ManagementModel &man_model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    SPDLOG_TRACE("PurchaseOrdersViewModel::PurchaseOrdersViewModel");
    getUserById = [&usr_model](const std::string userId){
        return usr_model.getUsernameById(userId);
    };
    getSupplierById = [&man_model](const std::string supplierId){
        return man_model.getSupplierById(supplierId);
    };

    connect(&m_model, &PurchaseOrdersModel::ordersChanged, this,
            &PurchaseOrdersViewModel::onOrdersChanged);
    connect(&m_model, &PurchaseOrdersModel::errorOccurred, this,
            &PurchaseOrdersViewModel::errorOccurred);
}

QVector<DisplayData::PurchaseOrder> PurchaseOrdersViewModel::orders() const { return m_orders; }

void PurchaseOrdersViewModel::fetchOrders() { m_model.fetchOrders(); }

void PurchaseOrdersViewModel::addOrder(const DisplayData::PurchaseOrder &order)
{
    m_model.addOrder(convertToCommonOrder(order));
}

void PurchaseOrdersViewModel::editOrder(const DisplayData::PurchaseOrder &order)
{
    m_model.editOrder(convertToCommonOrder(order));
}

void PurchaseOrdersViewModel::deleteOrder(const QString &id) { m_model.deleteOrder(id); }

void PurchaseOrdersViewModel::onOrdersChanged()
{
    m_orders.clear();

    for (const auto &order : m_model.orders())
        m_orders.emplace_back(convertToDisplayOrder(order));

    emit ordersChanged();
}

DisplayData::PurchaseOrder PurchaseOrdersViewModel::convertToDisplayOrder(const Common::Entities::PurchaseOrder &commonOrder) const
{
    DisplayData::PurchaseOrder displayOrder;
    displayOrder.id       = QString::fromStdString(commonOrder.id);
    displayOrder.date = QString::fromStdString(commonOrder.date);
    displayOrder.userId = QString::fromStdString(commonOrder.userId);
    displayOrder.username = getUserById(commonOrder.userId);
    displayOrder.supplierId   = QString::fromStdString(commonOrder.supplierId);
    displayOrder.supplier = getSupplierById(commonOrder.supplierId);
    displayOrder.status = QString::fromStdString(commonOrder.status);

    return displayOrder;
}

Common::Entities::PurchaseOrder PurchaseOrdersViewModel::convertToCommonOrder(const DisplayData::PurchaseOrder &displayOrder) const
{
    Common::Entities::PurchaseOrder commonOrder;
    commonOrder.id       = displayOrder.id.toStdString();
    commonOrder.date = displayOrder.date.toStdString();
    commonOrder.userId = displayOrder.userId.toStdString();
    commonOrder.supplierId   = displayOrder.supplierId.toStdString();
    commonOrder.status = displayOrder.status.toStdString();

    return commonOrder;
}