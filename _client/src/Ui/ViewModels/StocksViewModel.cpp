#include "Ui/ViewModels/StocksViewModel.hpp"

#include "Ui/Models/StocksModel.hpp"

#include "common/SpdlogConfig.hpp"

StocksViewModel::StocksViewModel(StocksModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    SPDLOG_TRACE("StocksViewModel::StocksViewModel");

    connect(&m_model, &StocksModel::stocksChanged, this, &StocksViewModel::rebuildDisplayState);
    connect(&m_model, &StocksModel::productTypesChanged, this,
            &StocksViewModel::rebuildDisplayState);
    connect(&m_model, &StocksModel::errorOccurred, this, &StocksViewModel::errorOccurred);
}

QVector<DisplayData::InventoryStock> StocksViewModel::stocks() const { return m_stocks; }

QVector<DisplayData::ProductType> StocksViewModel::productTypes() const { return m_productTypes; }

void StocksViewModel::fetchStocks() { m_model.fetchStocks(); }

void StocksViewModel::fetchProductTypes() { m_model.fetchProductTypes(); }

void StocksViewModel::createStock(const DisplayData::InventoryStock &stock)
{
    m_model.createStock(convertToCommonStock(stock));
}

void StocksViewModel::editStock(const DisplayData::InventoryStock &stock)
{
    m_model.editStock(convertToCommonStock(stock));
}

void StocksViewModel::deleteStock(const QString &id) { m_model.deleteStock(id); }

void StocksViewModel::rebuildDisplayState()
{
    m_productTypes.clear();
    for (const auto &productType : m_model.productTypes()) {
        m_productTypes.emplace_back(convertToDisplayProductType(productType));
    }

    m_stocks.clear();
    for (const auto &stock : m_model.stocks()) {
        DisplayData::InventoryStock displayStock;
        displayStock.id                = QString::fromStdString(stock.id);
        displayStock.productTypeId     = QString::fromStdString(stock.productTypeId);
        displayStock.productLabel      = productTypeLabelForId(displayStock.productTypeId);
        displayStock.quantityAvailable = QString::fromStdString(stock.quantityAvailable);
        displayStock.employeeId        = QString::fromStdString(stock.employeeId);
        m_stocks.emplace_back(displayStock);
    }

    emit stocksChanged();
}

DisplayData::ProductType
StocksViewModel::convertToDisplayProductType(const Common::Entities::ProductType &productType) const
{
    DisplayData::ProductType displayProductType;
    displayProductType.id          = QString::fromStdString(productType.id);
    displayProductType.code        = QString::fromStdString(productType.code);
    displayProductType.barcode     = QString::fromStdString(productType.barcode);
    displayProductType.name        = QString::fromStdString(productType.name);
    displayProductType.description = QString::fromStdString(productType.description);
    displayProductType.lastPrice   = QString::fromStdString(productType.lastPrice);
    displayProductType.unit        = QString::fromStdString(productType.unit);
    displayProductType.isImported  =
        productType.isImported == "1" ? "Yes" : "No";

    return displayProductType;
}

Common::Entities::Inventory
StocksViewModel::convertToCommonStock(const DisplayData::InventoryStock &stock) const
{
    return Common::Entities::Inventory{.id = stock.id.toStdString(),
                                       .productTypeId = stock.productTypeId.toStdString(),
                                       .quantityAvailable = stock.quantityAvailable.toStdString(),
                                       .employeeId = stock.employeeId.toStdString()};
}

QString StocksViewModel::productTypeLabelForId(const QString &productTypeId) const
{
    for (const auto &productType : m_productTypes) {
        if (productType.id == productTypeId) {
            return productType.code + " | " + productType.name;
        }
    }

    return productTypeId;
}
