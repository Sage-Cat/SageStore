#include "StocksModel.hpp"

#include "Network/ApiManager.hpp"

#include "common/SpdlogConfig.hpp"

StocksModel::StocksModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("StocksModel::StocksModel");

    connect(&m_apiManager, &ApiManager::stocksList, this, &StocksModel::onStocksList);
    connect(&m_apiManager, &ApiManager::productTypesList, this, &StocksModel::onProductTypesList);
    connect(&m_apiManager, &ApiManager::stockCreated, this, &StocksModel::onStockCreated);
    connect(&m_apiManager, &ApiManager::stockEdited, this, &StocksModel::onStockEdited);
    connect(&m_apiManager, &ApiManager::stockDeleted, this, &StocksModel::onStockDeleted);
}

QVector<Common::Entities::Inventory> StocksModel::stocks() const { return m_stocks; }

QVector<Common::Entities::ProductType> StocksModel::productTypes() const { return m_productTypes; }

void StocksModel::fetchStocks() { m_apiManager.getStocks(); }

void StocksModel::fetchProductTypes() { m_apiManager.getProductTypes(); }

void StocksModel::createStock(const Common::Entities::Inventory &stock)
{
    m_apiManager.createStock(stock);
}

void StocksModel::editStock(const Common::Entities::Inventory &stock) { m_apiManager.editStock(stock); }

void StocksModel::deleteStock(const QString &id) { m_apiManager.deleteStock(id); }

void StocksModel::onStocksList(const std::vector<Common::Entities::Inventory> &stocks)
{
    m_stocks = QVector<Common::Entities::Inventory>(stocks.begin(), stocks.end());
    emit stocksChanged();
}

void StocksModel::onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes)
{
    m_productTypes =
        QVector<Common::Entities::ProductType>(productTypes.begin(), productTypes.end());
    emit productTypesChanged();
}

void StocksModel::onStockCreated()
{
    fetchStocks();
    emit stockCreated();
}

void StocksModel::onStockEdited()
{
    fetchStocks();
    emit stockEdited();
}

void StocksModel::onStockDeleted()
{
    fetchStocks();
    emit stockDeleted();
}
