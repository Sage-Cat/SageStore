#include "Ui/ViewModels/PurchaseOrdersRecordsViewModel.hpp"
#include "Ui/Models/PurchaseOrdersModel.hpp"
#include "Ui/Models/InventoryModel.hpp"

#include "common/SpdlogConfig.hpp"

PurchaseOrderRecordsViewModel::PurchaseOrderRecordsViewModel(PurchaseOrdersModel &model, InventoryModel &inv_model, QObject *parent)
    : BaseViewModel(model, parent), m_model(model)
{
    SPDLOG_TRACE("PurchaseOrderRecordsViewModel::PurchaseOrderRecordsViewModel");
    getPurchaseRecordInfoById = [&inv_model](const std::string productTypeId){
        return inv_model.getPurchaseRecordInfoById(productTypeId);
    };

    connect(&m_model, &PurchaseOrdersModel::orderRecordsChanged, this,
            &PurchaseOrderRecordsViewModel::onRecordsChanged);
    connect(&m_model, &PurchaseOrdersModel::errorOccurred, this,
            &PurchaseOrderRecordsViewModel::errorOccurred);
}

QVector<DisplayData::PurchaseOrderRecord> PurchaseOrderRecordsViewModel::records() const { return m_records; };

void PurchaseOrderRecordsViewModel::fetchRecords() { m_model.fetchOrderRecords(); }

void PurchaseOrderRecordsViewModel::addRecord(const DisplayData::PurchaseOrderRecord &record)
{
    m_model.createOrderRecord(convertToCommonRecord(record));
}

void PurchaseOrderRecordsViewModel::editRecord(const DisplayData::PurchaseOrderRecord &record)
{
    m_model.editOrderRecord(convertToCommonRecord(record));
}

void PurchaseOrderRecordsViewModel::deleteRecord(const QString &id) { m_model.deleteOrderRecord(id); }

void PurchaseOrderRecordsViewModel::onRecordsChanged()
{
    m_records.clear();

    for (const auto &record : m_model.orderRecords())
        m_records.emplace_back(convertToDisplayRecord(record));

    emit recordsChanged();
}

DisplayData::PurchaseOrderRecord PurchaseOrderRecordsViewModel::convertToDisplayRecord(const Common::Entities::PurchaseOrderRecord &commonRecord) const
{
    std::vector<QString> m_pInfo = getPurchaseRecordInfoById(commonRecord.productTypeId);
    DisplayData::PurchaseOrderRecord displayRecord;
    displayRecord.id = QString::fromStdString(commonRecord.id);
    displayRecord.productTypeId = QString::fromStdString(commonRecord.productTypeId);
    displayRecord.orderId = QString::fromStdString(commonRecord.orderId);
    displayRecord.code = m_pInfo[0];
    displayRecord.name = m_pInfo[2];
    displayRecord.quantity = QString::fromStdString(commonRecord.quantity);
    displayRecord.unit = m_pInfo[4];
    displayRecord.price = QString::fromStdString(commonRecord.price);
    displayRecord.barcode = m_pInfo[1];
    displayRecord.description = m_pInfo[3];
    displayRecord.isImported = m_pInfo[5];
    
    return displayRecord;
}

Common::Entities::PurchaseOrderRecord PurchaseOrderRecordsViewModel::convertToCommonRecord(const DisplayData::PurchaseOrderRecord &displayRecord) const
{
    Common::Entities::PurchaseOrderRecord commonRecord;
    commonRecord.id = displayRecord.id.toStdString();
    commonRecord.productTypeId = displayRecord.productTypeId.toStdString();
    commonRecord.orderId = displayRecord.orderId.toStdString();
    commonRecord.quantity = displayRecord.quantity.toStdString();
    commonRecord.price = displayRecord.price.toStdString();

    return commonRecord;
}