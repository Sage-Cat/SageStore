#pragma once

#include "BaseModel.hpp"

#include "common/Entities/Supplier.hpp"

#include <QObject>
#include <QVector>

class ApiManager;

class ManagementModel : public BaseModel {
    Q_OBJECT
public:
    explicit ManagementModel(ApiManager &apiManager, QObject *parent = nullptr);

    QString getSupplierById(const std::string &supplierId) const;

    QVector<Common::Entities::Supplier> suppliers() const;

signals:
    // Suppliers
    void suppliersChanged();
    void supplierAdded();
    void supplierEdited();
    void supplierDeleted();

public slots:
    // Suppliers
    void fetchSuppliers();
    void addSupplier(const Common::Entities::Supplier &supplier);
    void editSupplier(const Common::Entities::Supplier &supplier);
    void deleteSupplier(const QString &id);

private slots: // --- ApiManager signal handlers ---
    void onSupplierAdded();
    void onSupplierUpdated();
    void onSupplierDeleted();
    void onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers);

private:
    ApiManager &m_apiManager;

    QVector<Common::Entities::Supplier> m_suppliers;
};
