#pragma once

#include <QVector>

#include "Ui/Models/BaseModel.hpp"

#include "common/Entities/ProductType.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"

class ApiManager;

class SupplierCatalogModel : public BaseModel {
    Q_OBJECT

public:
    explicit SupplierCatalogModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::SuppliersProductInfo> supplierProducts() const;
    QVector<Common::Entities::Supplier> suppliers() const;
    QVector<Common::Entities::ProductType> productTypes() const;

signals:
    void supplierProductsChanged();
    void suppliersChanged();
    void productTypesChanged();
    void supplierProductCreated();
    void supplierProductEdited();
    void supplierProductDeleted();

public slots:
    void fetchAll();
    void fetchSupplierProducts();
    void fetchSuppliers();
    void fetchProductTypes();
    void createSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct);
    void editSupplierProduct(const Common::Entities::SuppliersProductInfo &supplierProduct);
    void deleteSupplierProduct(const QString &id);

private slots:
    void onSupplierProductsList(
        const std::vector<Common::Entities::SuppliersProductInfo> &supplierProducts);
    void onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers);
    void onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes);
    void onSupplierProductCreated();
    void onSupplierProductEdited();
    void onSupplierProductDeleted();

private:
    ApiManager &m_apiManager;
    QVector<Common::Entities::SuppliersProductInfo> m_supplierProducts;
    QVector<Common::Entities::Supplier> m_suppliers;
    QVector<Common::Entities::ProductType> m_productTypes;
};
