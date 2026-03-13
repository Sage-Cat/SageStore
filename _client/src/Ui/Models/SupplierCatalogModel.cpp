#include "Ui/Models/SupplierCatalogModel.hpp"

#include "Network/ApiManager.hpp"

SupplierCatalogModel::SupplierCatalogModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    connect(&m_apiManager, &ApiManager::errorOccurred, this, &SupplierCatalogModel::errorOccurred);
    connect(&m_apiManager, &ApiManager::supplierProductsList, this,
            &SupplierCatalogModel::onSupplierProductsList);
    connect(&m_apiManager, &ApiManager::suppliersList, this, &SupplierCatalogModel::onSuppliersList);
    connect(&m_apiManager, &ApiManager::productTypesList, this,
            &SupplierCatalogModel::onProductTypesList);
    connect(&m_apiManager, &ApiManager::supplierProductCreated, this,
            &SupplierCatalogModel::onSupplierProductCreated);
    connect(&m_apiManager, &ApiManager::supplierProductEdited, this,
            &SupplierCatalogModel::onSupplierProductEdited);
    connect(&m_apiManager, &ApiManager::supplierProductDeleted, this,
            &SupplierCatalogModel::onSupplierProductDeleted);
}

QVector<Common::Entities::SuppliersProductInfo> SupplierCatalogModel::supplierProducts() const
{
    return m_supplierProducts;
}

QVector<Common::Entities::Supplier> SupplierCatalogModel::suppliers() const { return m_suppliers; }

QVector<Common::Entities::ProductType> SupplierCatalogModel::productTypes() const
{
    return m_productTypes;
}

void SupplierCatalogModel::fetchAll()
{
    fetchSuppliers();
    fetchProductTypes();
    fetchSupplierProducts();
}

void SupplierCatalogModel::fetchSupplierProducts() { m_apiManager.getSupplierProducts(); }

void SupplierCatalogModel::fetchSuppliers() { m_apiManager.getSuppliers(); }

void SupplierCatalogModel::fetchProductTypes() { m_apiManager.getProductTypes(); }

void SupplierCatalogModel::createSupplierProduct(
    const Common::Entities::SuppliersProductInfo &supplierProduct)
{
    m_apiManager.createSupplierProduct(supplierProduct);
}

void SupplierCatalogModel::editSupplierProduct(
    const Common::Entities::SuppliersProductInfo &supplierProduct)
{
    m_apiManager.editSupplierProduct(supplierProduct);
}

void SupplierCatalogModel::deleteSupplierProduct(const QString &id)
{
    m_apiManager.deleteSupplierProduct(id);
}

void SupplierCatalogModel::onSupplierProductsList(
    const std::vector<Common::Entities::SuppliersProductInfo> &supplierProducts)
{
    m_supplierProducts =
        QVector<Common::Entities::SuppliersProductInfo>(supplierProducts.begin(),
                                                        supplierProducts.end());
    emit supplierProductsChanged();
}

void SupplierCatalogModel::onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers)
{
    m_suppliers = QVector<Common::Entities::Supplier>(suppliers.begin(), suppliers.end());
    emit suppliersChanged();
}

void SupplierCatalogModel::onProductTypesList(
    const std::vector<Common::Entities::ProductType> &productTypes)
{
    m_productTypes = QVector<Common::Entities::ProductType>(productTypes.begin(), productTypes.end());
    emit productTypesChanged();
}

void SupplierCatalogModel::onSupplierProductCreated()
{
    fetchSupplierProducts();
    emit supplierProductCreated();
}

void SupplierCatalogModel::onSupplierProductEdited()
{
    fetchSupplierProducts();
    emit supplierProductEdited();
}

void SupplierCatalogModel::onSupplierProductDeleted()
{
    fetchSupplierProducts();
    emit supplierProductDeleted();
}
