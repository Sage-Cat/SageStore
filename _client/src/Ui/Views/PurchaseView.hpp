#pragma once

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QVector>
#include <QWidget>

#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"

class PurchaseViewModel;

class PurchaseView : public QWidget {
    Q_OBJECT

public:
    enum class Section { Orders, Receipts };

    explicit PurchaseView(PurchaseViewModel &viewModel, QWidget *parent = nullptr);

    void showSection(Section section);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onOrdersChanged();
    void onOrderRecordsChanged();
    void onReferenceDataChanged();
    void onPurchaseReceiptPosted();

    void onAddOrder();
    void onDeleteOrder();
    void onAddRecord();
    void onDeleteRecord();
    void onReceiveOrder();
    void onOrdersFilterChanged(const QString &text);
    void onReceiptsFilterChanged(const QString &text);
    void onOrdersSelectionChanged();
    void onOrderRecordsSelectionChanged();
    void onReceiptOrdersSelectionChanged();
    void onOrdersTableItemChanged(QTableWidgetItem *item);
    void onOrderRecordsTableItemChanged(QTableWidgetItem *item);

private:
    enum class RecordsContext { None, Orders, Receipts };

    void setupUi();
    void applyOrdersFilter();
    void applyReceiptsFilter();
    void fillOrdersTable(
        QTableWidget *table, const QVector<Common::Entities::PurchaseOrder> &orders);
    void fillRecordsTable(QTableWidget *table,
                          const QVector<Common::Entities::PurchaseOrderRecord> &records);
    void updateOrdersActions();
    void updateReceiptActions();
    void updateStatus();
    void refreshEmployeeOptions();
    QString selectedOrderId(QTableWidget *table) const;
    QString selectedOrderStatus(QTableWidget *table) const;
    void persistOrderRow(int row);
    void persistOrderRecordRow(int row);

    PurchaseViewModel &m_viewModel;
    QTabWidget *m_tabs{nullptr};

    QLineEdit *m_ordersFilterField{nullptr};
    QPushButton *m_addOrderButton{nullptr};
    QPushButton *m_deleteOrderButton{nullptr};
    QTableWidget *m_ordersTable{nullptr};
    QPushButton *m_addRecordButton{nullptr};
    QPushButton *m_deleteRecordButton{nullptr};
    QTableWidget *m_orderRecordsTable{nullptr};
    QLabel *m_ordersStatusLabel{nullptr};

    QLineEdit *m_receiptsFilterField{nullptr};
    QComboBox *m_receiptEmployeeBox{nullptr};
    QPushButton *m_receiveButton{nullptr};
    QTableWidget *m_receiptOrdersTable{nullptr};
    QTableWidget *m_receiptRecordsTable{nullptr};
    QLabel *m_receiptsStatusLabel{nullptr};

    QVector<Common::Entities::PurchaseOrder> m_allOrders;
    QVector<Common::Entities::PurchaseOrderRecord> m_currentRecords;
    RecordsContext m_recordsContext{RecordsContext::None};
    bool m_isSyncingTables{false};
};
