#pragma once

#include <QObject>
#include <QVector>

#include "Ui/DisplayData/ProductType.hpp"
#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/ProductType.hpp"

class ProductTypesModel;

class ProductTypesViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit ProductTypesViewModel(ProductTypesModel &model, QObject *parent = nullptr);

    QVector<DisplayData::ProductType> productTypes() const;

signals:
    void productTypesChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchProductTypes();
    void createProductType(const DisplayData::ProductType &productType);
    void editProductType(const DisplayData::ProductType &productType);
    void deleteProductType(const QString &id);

private slots:
    void onProductTypesChanged();

private:
    DisplayData::ProductType
    convertToDisplayProductType(const Common::Entities::ProductType &productType) const;
    Common::Entities::ProductType
    convertToCommonProductType(const DisplayData::ProductType &productType) const;

    ProductTypesModel &m_model;
    QVector<DisplayData::ProductType> m_productTypes;
};
