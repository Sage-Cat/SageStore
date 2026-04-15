#pragma once

#include <functional>

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QVector>
#include <QWidget>

#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"

class SalesViewModel;

class SalesView : public QWidget {
    Q_OBJECT

public:
    enum class Section { Orders, Invoices };
    using InvoiceExportPathProvider =
        std::function<QString(QWidget *, const QString &, const QString &, const QString &)>;

    explicit SalesView(SalesViewModel &viewModel, QWidget *parent = nullptr);

    void showSection(Section section);
    void setInvoiceExportPathProvider(InvoiceExportPathProvider provider);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onOrdersChanged();
    void onOrderRecordsChanged();
    void onReferenceDataChanged();
    void onAddOrder();
    void onDeleteOrder();
    void onAddRecord();
    void onDeleteRecord();
    void onOrdersSelectionChanged();
    void onOrderRecordsSelectionChanged();
    void onInvoiceSelectionChanged();
    void onOrdersFilterChanged(const QString &text);
    void onInvoicesFilterChanged(const QString &text);
    void onExportInvoice();
    void onOrdersTableItemChanged(QTableWidgetItem *item);
    void onOrderRecordsTableItemChanged(QTableWidgetItem *item);

private:
    enum class RecordsContext { None, Orders, Invoices };

    void setupUi();
    void applyOrdersFilter();
    void applyInvoicesFilter();
    void fillOrdersTable(QTableWidget *table, const QVector<Common::Entities::SaleOrder> &orders);
    void fillRecordsTable(QTableWidget *table,
                          const QVector<Common::Entities::SalesOrderRecord> &records);
    void updateActions();
    void updateInvoicePreview();
    QString selectedOrderId(QTableWidget *table) const;
    void persistOrderRow(int row);
    void persistOrderRecordRow(int row);

    SalesViewModel &m_viewModel;
    QTabWidget *m_tabs{nullptr};
    QLineEdit *m_ordersFilterField{nullptr};
    QPushButton *m_addOrderButton{nullptr};
    QPushButton *m_deleteOrderButton{nullptr};
    QTableWidget *m_ordersTable{nullptr};
    QPushButton *m_addRecordButton{nullptr};
    QPushButton *m_deleteRecordButton{nullptr};
    QTableWidget *m_orderRecordsTable{nullptr};
    QLabel *m_ordersStatusLabel{nullptr};

    QLineEdit *m_invoicesFilterField{nullptr};
    QPushButton *m_exportInvoiceButton{nullptr};
    QTableWidget *m_invoiceOrdersTable{nullptr};
    QTableWidget *m_invoiceRecordsTable{nullptr};
    QPlainTextEdit *m_invoicePreview{nullptr};
    QLabel *m_invoicesStatusLabel{nullptr};
    InvoiceExportPathProvider m_invoiceExportPathProvider;

    QVector<Common::Entities::SaleOrder> m_allOrders;
    QVector<Common::Entities::SalesOrderRecord> m_currentRecords;
    RecordsContext m_recordsContext{RecordsContext::None};
    bool m_isSyncingTables{false};
};
