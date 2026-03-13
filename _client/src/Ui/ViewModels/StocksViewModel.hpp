#pragma once

#include <QObject>
#include <QVector>

#include "Ui/DisplayData/InventoryStock.hpp"
#include "Ui/DisplayData/ProductType.hpp"
#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/Entities/ProductType.hpp"

class StocksModel;

class StocksViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit StocksViewModel(StocksModel &model, QObject *parent = nullptr);

    QVector<DisplayData::InventoryStock> stocks() const;
    QVector<DisplayData::ProductType> productTypes() const;

signals:
    void stocksChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchStocks();
    void fetchProductTypes();
    void createStock(const DisplayData::InventoryStock &stock);
    void editStock(const DisplayData::InventoryStock &stock);
    void deleteStock(const QString &id);

private slots:
    void rebuildDisplayState();

private:
    DisplayData::ProductType
    convertToDisplayProductType(const Common::Entities::ProductType &productType) const;
    Common::Entities::Inventory
    convertToCommonStock(const DisplayData::InventoryStock &stock) const;
    QString productTypeLabelForId(const QString &productTypeId) const;

    StocksModel &m_model;
    QVector<DisplayData::InventoryStock> m_stocks;
    QVector<DisplayData::ProductType> m_productTypes;
};
