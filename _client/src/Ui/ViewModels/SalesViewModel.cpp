#include "Ui/ViewModels/SalesViewModel.hpp"

#include <algorithm>

#include <QTextStream>

#include "Ui/Models/SalesModel.hpp"

SalesViewModel::SalesViewModel(SalesModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &SalesModel::ordersChanged, this, &SalesViewModel::ordersChanged);
    connect(&m_model, &SalesModel::orderRecordsChanged, this, &SalesViewModel::orderRecordsChanged);
    connect(&m_model, &SalesModel::usersChanged, this, &SalesViewModel::referenceDataChanged);
    connect(&m_model, &SalesModel::contactsChanged, this, &SalesViewModel::referenceDataChanged);
    connect(&m_model, &SalesModel::employeesChanged, this, &SalesViewModel::referenceDataChanged);
    connect(&m_model, &SalesModel::productTypesChanged, this, &SalesViewModel::referenceDataChanged);
    connect(&m_model, &SalesModel::errorOccurred, this, &SalesViewModel::errorOccurred);
}

QVector<Common::Entities::SaleOrder> SalesViewModel::orders() const { return m_model.orders(); }

QVector<Common::Entities::SalesOrderRecord> SalesViewModel::orderRecords() const
{
    return m_model.orderRecords();
}

QVector<Common::Entities::User> SalesViewModel::users() const { return m_model.users(); }

QVector<Common::Entities::Contact> SalesViewModel::contacts() const { return m_model.contacts(); }

QVector<Common::Entities::Employee> SalesViewModel::employees() const
{
    return m_model.employees();
}

QVector<Common::Entities::ProductType> SalesViewModel::productTypes() const
{
    return m_model.productTypes();
}

QString SalesViewModel::userLabel(const QString &userId) const
{
    for (const auto &user : m_model.users()) {
        if (QString::fromStdString(user.id) == userId) {
            return QString::fromStdString(user.username);
        }
    }
    return userId;
}

QString SalesViewModel::contactLabel(const QString &contactId) const
{
    for (const auto &contact : m_model.contacts()) {
        if (QString::fromStdString(contact.id) == contactId) {
            return QString::fromStdString(contact.name);
        }
    }
    return contactId;
}

QString SalesViewModel::employeeLabel(const QString &employeeId) const
{
    for (const auto &employee : m_model.employees()) {
        if (QString::fromStdString(employee.id) == employeeId) {
            return QString::fromStdString(employee.name);
        }
    }
    return employeeId;
}

QString SalesViewModel::productTypeLabel(const QString &productTypeId) const
{
    for (const auto &productType : m_model.productTypes()) {
        if (QString::fromStdString(productType.id) == productTypeId) {
            return QString::fromStdString(productType.code) + " | " +
                   QString::fromStdString(productType.name);
        }
    }
    return productTypeId;
}

QString SalesViewModel::buildInvoicePreview(const QString &orderId) const
{
    const auto orderIt = std::find_if(m_model.orders().begin(), m_model.orders().end(),
                                      [&](const auto &order) {
                                          return QString::fromStdString(order.id) == orderId;
                                      });
    if (orderIt == m_model.orders().end()) {
        return {};
    }

    QString preview;
    QTextStream stream(&preview);
    stream << "SageStore Invoice\n";
    stream << "Order ID: " << QString::fromStdString(orderIt->id) << "\n";
    stream << "Date: " << QString::fromStdString(orderIt->date) << "\n";
    stream << "Customer: " << contactLabel(QString::fromStdString(orderIt->contactId)) << "\n";
    stream << "Handled by: " << employeeLabel(QString::fromStdString(orderIt->employeeId)) << "\n";
    stream << "Status: " << QString::fromStdString(orderIt->status) << "\n\n";
    stream << "Items:\n";

    double total = 0.0;
    for (const auto &record : m_model.orderRecords()) {
        const double price = QString::fromStdString(record.price).toDouble();
        const int quantity = QString::fromStdString(record.quantity).toInt();
        total += price * static_cast<double>(quantity);
        stream << " - " << productTypeLabel(QString::fromStdString(record.productTypeId)) << " | "
               << quantity << " x " << price << " = " << (price * quantity) << "\n";
    }

    stream << "\nTotal: " << total << "\n";
    return preview;
}

void SalesViewModel::fetchAll() { m_model.fetchAll(); }

void SalesViewModel::fetchOrders() { m_model.fetchOrders(); }

void SalesViewModel::fetchOrderRecords(const QString &orderId)
{
    m_model.fetchOrderRecords(orderId);
}

void SalesViewModel::createOrder(const Common::Entities::SaleOrder &order)
{
    m_model.createOrder(order);
}

void SalesViewModel::editOrder(const Common::Entities::SaleOrder &order)
{
    m_model.editOrder(order);
}

void SalesViewModel::deleteOrder(const QString &id) { m_model.deleteOrder(id); }

void SalesViewModel::createOrderRecord(const Common::Entities::SalesOrderRecord &record)
{
    m_model.createOrderRecord(record);
}

void SalesViewModel::editOrderRecord(const Common::Entities::SalesOrderRecord &record)
{
    m_model.editOrderRecord(record);
}

void SalesViewModel::deleteOrderRecord(const QString &id) { m_model.deleteOrderRecord(id); }
