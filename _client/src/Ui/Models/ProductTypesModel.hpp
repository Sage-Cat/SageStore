#pragma once

#include <QObject>
#include <QVector>

#include "BaseModel.hpp"

#include "common/Entities/ProductType.hpp"

class ApiManager;

class ProductTypesModel : public BaseModel {
    Q_OBJECT

public:
    explicit ProductTypesModel(ApiManager &apiManager, QObject *parent = nullptr);

    QVector<Common::Entities::ProductType> productTypes() const;

signals:
    void productTypesChanged();
    void productTypeCreated();
    void productTypeEdited();
    void productTypeDeleted();

public slots:
    void fetchProductTypes();
    void createProductType(const Common::Entities::ProductType &productType);
    void editProductType(const Common::Entities::ProductType &productType);
    void deleteProductType(const QString &id);

private slots:
    void onProductTypesList(const std::vector<Common::Entities::ProductType> &productTypes);
    void onProductTypeCreated();
    void onProductTypeEdited();
    void onProductTypeDeleted();

private:
    ApiManager &m_apiManager;
    QVector<Common::Entities::ProductType> m_productTypes;
};
