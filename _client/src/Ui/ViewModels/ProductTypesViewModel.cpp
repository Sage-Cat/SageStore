#include "Ui/ViewModels/ProductTypesViewModel.hpp"

#include "Ui/Models/ProductTypesModel.hpp"

#include "common/SpdlogConfig.hpp"

ProductTypesViewModel::ProductTypesViewModel(ProductTypesModel &model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    SPDLOG_TRACE("ProductTypesViewModel::ProductTypesViewModel");

    connect(&m_model, &ProductTypesModel::productTypesChanged, this,
            &ProductTypesViewModel::onProductTypesChanged);
    connect(&m_model, &ProductTypesModel::errorOccurred, this,
            &ProductTypesViewModel::errorOccurred);
}

QVector<DisplayData::ProductType> ProductTypesViewModel::productTypes() const
{
    return m_productTypes;
}

void ProductTypesViewModel::fetchProductTypes() { m_model.fetchProductTypes(); }

void ProductTypesViewModel::createProductType(const DisplayData::ProductType &productType)
{
    m_model.createProductType(convertToCommonProductType(productType));
}

void ProductTypesViewModel::editProductType(const DisplayData::ProductType &productType)
{
    m_model.editProductType(convertToCommonProductType(productType));
}

void ProductTypesViewModel::deleteProductType(const QString &id) { m_model.deleteProductType(id); }

void ProductTypesViewModel::onProductTypesChanged()
{
    m_productTypes.clear();

    for (const auto &productType : m_model.productTypes()) {
        m_productTypes.emplace_back(convertToDisplayProductType(productType));
    }

    emit productTypesChanged();
}

DisplayData::ProductType ProductTypesViewModel::convertToDisplayProductType(
    const Common::Entities::ProductType &productType) const
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

Common::Entities::ProductType ProductTypesViewModel::convertToCommonProductType(
    const DisplayData::ProductType &productType) const
{
    Common::Entities::ProductType commonProductType;
    commonProductType.id          = productType.id.toStdString();
    commonProductType.code        = productType.code.toStdString();
    commonProductType.barcode     = productType.barcode.toStdString();
    commonProductType.name        = productType.name.toStdString();
    commonProductType.description = productType.description.toStdString();
    commonProductType.lastPrice   = productType.lastPrice.toStdString();
    commonProductType.unit        = productType.unit.toStdString();
    commonProductType.isImported =
        (productType.isImported.compare("Yes", Qt::CaseInsensitive) == 0) ? "1" : "0";

    return commonProductType;
}
