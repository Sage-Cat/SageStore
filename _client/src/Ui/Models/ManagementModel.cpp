#include "ManagementModel.hpp"

#include "Network/ApiManager.hpp"

#include "common/SpdlogConfig.hpp"

ManagementModel::ManagementModel(ApiManager &apiManager, QObject *parent)
    : BaseModel(parent), m_apiManager(apiManager)
{
    SPDLOG_TRACE("ManagementModel::ManagementModel");

    connect(&m_apiManager, &ApiManager::supplierAdded, this, &ManagementModel::onSupplierAdded);
    connect(&m_apiManager, &ApiManager::supplierEdited, this, &ManagementModel::onSupplierUpdated);
    connect(&m_apiManager, &ApiManager::supplierDeleted, this, &ManagementModel::onSupplierDeleted);
    connect(&m_apiManager, &ApiManager::suppliersList, this, &ManagementModel::onSuppliersList);

    // Fetch data on startup
    SPDLOG_TRACE("ManagementModel | On startup fetch data");
    fetchSuppliers();
}


QVector<Common::Entities::Supplier> ManagementModel::suppliers() const { return m_suppliers; }

void ManagementModel::fetchSuppliers() 
{ 
    m_apiManager.getSuppliers(); 
}

void ManagementModel::addSupplier(const Common::Entities::Supplier &supplier) 
{ 
    m_apiManager.createSupplier(supplier); 
}

void ManagementModel::editSupplier(const Common::Entities::Supplier &supplier) 
{ 
    m_apiManager.editSupplier(supplier); 
}

void ManagementModel::deleteSupplier(const QString &id) 
{ 
    m_apiManager.deleteSupplier(id); 
}

void ManagementModel::onSuppliersList(const std::vector<Common::Entities::Supplier> &suppliers)
{
    m_suppliers = QVector<Common::Entities::Supplier>(suppliers.begin(), suppliers.end());
    emit suppliersChanged();
}

void ManagementModel::onSupplierAdded()
{
    fetchSuppliers();
    emit supplierAdded();
}

void ManagementModel::onSupplierUpdated()
{
    fetchSuppliers();
    emit supplierEdited();
}

void ManagementModel::onSupplierDeleted()
{
    fetchSuppliers();
    emit supplierDeleted();
}

QString ManagementModel::getSupplierById(const std::string &supplierId) const
{
    for(const auto &supplier : m_suppliers) {
        if(supplier.id == supplierId) {
            return QString::fromStdString(supplier.name);
        }
    }
    return QString();
}
