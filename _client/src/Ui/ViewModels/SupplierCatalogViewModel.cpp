#include "Ui/ViewModels/SupplierCatalogViewModel.hpp"

#include "Ui/Models/SupplierCatalogModel.hpp"

SupplierCatalogViewModel::SupplierCatalogViewModel(SupplierCatalogModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    connect(&m_model, &SupplierCatalogModel::supplierProductsChanged, this,
            &SupplierCatalogViewModel::supplierProductsChanged);
    connect(&m_model, &SupplierCatalogModel::suppliersChanged, this,
            &SupplierCatalogViewModel::suppliersChanged);
    connect(&m_model, &SupplierCatalogModel::productTypesChanged, this,
            &SupplierCatalogViewModel::productTypesChanged);
    connect(&m_model, &SupplierCatalogModel::errorOccurred, this,
            &SupplierCatalogViewModel::errorOccurred);
}

QVector<Common::Entities::SuppliersProductInfo> SupplierCatalogViewModel::supplierProducts() const
{
    return m_model.supplierProducts();
}

QVector<Common::Entities::Supplier> SupplierCatalogViewModel::suppliers() const
{
    return m_model.suppliers();
}

QVector<Common::Entities::ProductType> SupplierCatalogViewModel::productTypes() const
{
    return m_model.productTypes();
}

QString SupplierCatalogViewModel::supplierLabel(const QString &supplierId) const
{
    for (const auto &supplier : m_model.suppliers()) {
        if (QString::fromStdString(supplier.id) == supplierId) {
            return QString::fromStdString(supplier.name);
        }
    }
    return supplierId;
}

QString SupplierCatalogViewModel::productTypeLabel(const QString &productTypeId) const
{
    for (const auto &productType : m_model.productTypes()) {
        if (QString::fromStdString(productType.id) == productTypeId) {
            return QString::fromStdString(productType.code) + " | " +
                   QString::fromStdString(productType.name);
        }
    }
    return productTypeId;
}

void SupplierCatalogViewModel::fetchAll() { m_model.fetchAll(); }

void SupplierCatalogViewModel::createSupplierProduct(
    const Common::Entities::SuppliersProductInfo &supplierProduct)
{
    m_model.createSupplierProduct(supplierProduct);
}

void SupplierCatalogViewModel::editSupplierProduct(
    const Common::Entities::SuppliersProductInfo &supplierProduct)
{
    m_model.editSupplierProduct(supplierProduct);
}

void SupplierCatalogViewModel::deleteSupplierProduct(const QString &id)
{
    m_model.deleteSupplierProduct(id);
}
