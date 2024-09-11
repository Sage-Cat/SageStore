#pragma once

#include <QObject>
#include <QVector>

#include "Ui/DisplayData/Entities.hpp"
#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/Entities/PurchaseOrderRecord.hpp"


class PurchaseOrdersModel;
class InventoryModel;

class PurchaseOrderRecordsViewModel : public BaseViewModel {
    Q_OBJECT
public:
    explicit PurchaseOrderRecordsViewModel(PurchaseOrdersModel &model, InventoryModel &inv_model, QObject *parent = nullptr);

    QVector<DisplayData::PurchaseOrderRecord> records() const;

signals:
    void recordsChanged();
    void errorOccurred(const QString &errorMessage);

public slots:
    void fetchRecords();
    void addRecord(const DisplayData::PurchaseOrderRecord &record);
    void editRecord(const DisplayData::PurchaseOrderRecord &record);
    void deleteRecord(const QString &id);

private slots:
    void onRecordsChanged();

private:
    DisplayData::PurchaseOrderRecord convertToDisplayRecord(const Common::Entities::PurchaseOrderRecord &commonRecord) const;
    Common::Entities::PurchaseOrderRecord convertToCommonRecord(const DisplayData::PurchaseOrderRecord &displayRecord) const;

private:
    PurchaseOrdersModel &m_model;
    QVector<DisplayData::PurchaseOrderRecord> m_records;

    std::function<std::vector<QString>(const std::string)> getPurchaseRecordInfoById;

};