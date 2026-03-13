#pragma once

#include <QObject>
#include <QVector>

#include "BaseModel.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/Entities/ProductType.hpp"

class ApiManager;

class StocksModel : public BaseModel {
    Q_OBJECT

public:
    explicit StocksModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::Inventory> stocks() const;
    QVector<Common::Entities::ProductType> productTypes() const;

signals:
    void stocksChanged();
    void productTypesChanged();
    void stockCreated();
    void stockEdited();
    void stockDeleted();

public slots:
    void fetchStocks();
    void fetchProductTypes();
    void createStock(const Common::Entities::Inventory &stock);
    void editStock(const Common::Entities::Inventory &stock);
    void deleteStock(const QString &id);

private slots:
    void onStocksList(const std::vector<Common::Entities::Inventory> &stocks);
    void onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes);
    void onStockCreated();
    void onStockEdited();
    void onStockDeleted();

private:
    ApiManager &m_apiManager;
    QVector<Common::Entities::Inventory> m_stocks;
    QVector<Common::Entities::ProductType> m_productTypes;
};
