#pragma once

#include "BaseModel.hpp"

#include "common/Entities/ProductType.hpp"

#include <QObject>
#include <QVector>

class ApiManager;

class InventoryModel : public BaseModel {
    Q_OBJECT
public:
    explicit InventoryModel(ApiManager &apiManager, QObject *parent = nullptr);

    std::vector<QString> getPurchaseRecordInfoById(const std::string &pInfoId) const;

    QVector<Common::Entities::ProductType> products() const;

signals:
    // Products
    void productsChanged();
    void productAdded();
    void productEdited();
    void productDeleted();

public slots:
    // Products
    void fetchProducts();
    void addProduct(const Common::Entities::ProductType &productType);
    void editProduct(const Common::Entities::ProductType &productType);
    void deleteProduct(const QString &id);

private slots: // --- ApiManager signal handlers ---
    void onProductTypeAdded();
    void onProductTypeUpdated();
    void onProductTypeDeleted();
    void onProductTypesList(const std::vector<Common::Entities::ProductType> &products);

private:
    ApiManager &m_apiManager;

    QVector<Common::Entities::ProductType> m_products;
};