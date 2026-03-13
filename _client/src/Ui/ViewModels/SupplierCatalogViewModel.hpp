#pragma once

#include <QVector>

#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/ProductType.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"

class SupplierCatalogModel;

class SupplierCatalogViewModel : public BaseViewModel {
    Q_OBJECT

public:
    explicit SupplierCatalogViewModel(SupplierCatalogModel &model, QObject *parent = nullptr);

    QVector<Common::Entities::SuppliersProductInfo> supplierProducts() const;
    QVector<Common::Entities::Supplier> suppliers() const;
    QVector<Common::Entities::ProductType> productTypes() const;

    QString supplierLabel(const QString &supplierId) const;
    QString productTypeLabel(const QString &productTypeId) const;

signals:
    void supplierProductsChanged();
    void suppliersChanged();
    void productTypesChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchAll();
    void createSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct);
    void editSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct);
    void deleteSupplierProduct(const QString &id);

private:
    SupplierCatalogModel &m_model;
};
