#include "ProductTypesModel.hpp"

#include "Network/ApiManager.hpp"

#include "common/SpdlogConfig.hpp"

ProductTypesModel::ProductTypesModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("ProductTypesModel::ProductTypesModel");

    connect(&m_apiManager, &ApiManager::productTypesList, this,
            &ProductTypesModel::onProductTypesList);
    connect(&m_apiManager, &ApiManager::productTypeCreated, this,
            &ProductTypesModel::onProductTypeCreated);
    connect(&m_apiManager, &ApiManager::productTypeEdited, this,
            &ProductTypesModel::onProductTypeEdited);
    connect(&m_apiManager, &ApiManager::productTypeDeleted, this,
            &ProductTypesModel::onProductTypeDeleted);
}

QVector<Common::Entities::ProductType> ProductTypesModel::productTypes() const
{
    return m_productTypes;
}

void ProductTypesModel::fetchProductTypes() { m_apiManager.getProductTypes(); }

void ProductTypesModel::createProductType(const Common::Entities::ProductType &productType)
{
    m_apiManager.createProductType(productType);
}

void ProductTypesModel::editProductType(const Common::Entities::ProductType &productType)
{
    m_apiManager.editProductType(productType);
}

void ProductTypesModel::deleteProductType(const QString &id) { m_apiManager.deleteProductType(id); }

void ProductTypesModel::onProductTypesList(
    const std::vector<Common::Entities::ProductType> &productTypes)
{
    m_productTypes = QVector<Common::Entities::ProductType>(productTypes.begin(), productTypes.end());
    emit productTypesChanged();
}

void ProductTypesModel::onProductTypeCreated()
{
    fetchProductTypes();
    emit productTypeCreated();
}

void ProductTypesModel::onProductTypeEdited()
{
    fetchProductTypes();
    emit productTypeEdited();
}

void ProductTypesModel::onProductTypeDeleted()
{
    fetchProductTypes();
    emit productTypeDeleted();
}
