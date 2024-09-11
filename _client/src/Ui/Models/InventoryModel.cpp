#include "InventoryModel.hpp"

#include "Network/ApiManager.hpp"

#include "common/SpdlogConfig.hpp"

InventoryModel::InventoryModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("InventoryModel::InventoryModel");

    connect(&m_apiManager, &ApiManager::productTypeAdded, this, &InventoryModel::onProductTypeAdded);
    connect(&m_apiManager, &ApiManager::productTypeEdited, this, &InventoryModel::onProductTypeUpdated);
    connect(&m_apiManager, &ApiManager::productTypeDeleted, this, &InventoryModel::onProductTypeDeleted);
    connect(&m_apiManager, &ApiManager::productTypesList, this, &InventoryModel::onProductTypesList);

    // Fetch data on startup
    SPDLOG_TRACE("InventoryModel | On startup fetch data");
    fetchProducts();
}


QVector<Common::Entities::ProductType> InventoryModel::products() const { return m_products; }

void InventoryModel::fetchProducts() 
{ 
    m_apiManager.getProductTypes(); 
}

void InventoryModel::addProduct(const Common::Entities::ProductType &productType) 
{ 
    m_apiManager.createProductType(productType); 
}

void InventoryModel::editProduct(const Common::Entities::ProductType &productType) 
{ 
    m_apiManager.editProductType(productType); 
}

void InventoryModel::deleteProduct(const QString &id) 
{ 
    m_apiManager.deleteProductType(id); 
}

void InventoryModel::onProductTypesList(const std::vector<Common::Entities::ProductType> &products)
{
    m_products = QVector<Common::Entities::ProductType>(products.begin(), products.end());
    emit productsChanged();
}

void InventoryModel::onProductTypeAdded()
{
    fetchProducts();
    emit productAdded();
}

void InventoryModel::onProductTypeUpdated()
{
    fetchProducts();
    emit productEdited();
}

void InventoryModel::onProductTypeDeleted()
{
    fetchProducts();
    emit productDeleted();
}

std::vector<QString> InventoryModel::getPurchaseRecordInfoById(const std::string &pInfoId) const
{
    for(const auto &pInfo : m_products) {
        if(pInfo.id == pInfoId) {
            return std::vector<QString>{QString::fromStdString(pInfo.code), 
                                        QString::fromStdString(pInfo.barcode),
                                        QString::fromStdString(pInfo.name),
                                        QString::fromStdString(pInfo.description),
                                        QString::fromStdString(pInfo.unit),
                                        QString::fromStdString(pInfo.isImported)};
        }
    }
    return std::vector<QString>();
}