#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
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

    explicit SalesView(SalesViewModel &viewModel, QWidget *parent = nullptr);

    void showSection(Section section);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onOrdersChanged();
    void onOrderRecordsChanged();
    void onReferenceDataChanged();
    void onAddOrder();
    void onEditOrder();
    void onDeleteOrder();
    void onAddRecord();
    void onEditRecord();
    void onDeleteRecord();
    void onOrdersSelectionChanged();
    void onInvoiceSelectionChanged();
    void onOrdersFilterChanged(const QString &text);
    void onInvoicesFilterChanged(const QString &text);
    void onExportInvoice();

private:
    enum class RecordsContext { None, Orders, Invoices };

    void setupUi();
    void applyOrdersFilter();
    void applyInvoicesFilter();
    void fillOrdersTable(QTableWidget *table, const QVector<Common::Entities::SaleOrder> &orders) const;
    void fillRecordsTable(QTableWidget *table,
                          const QVector<Common::Entities::SalesOrderRecord> &records) const;
    void updateActions();
    void updateInvoicePreview();
    QString selectedOrderId(QTableWidget *table) const;
    bool showOrderDialog(Common::Entities::SaleOrder &order, const QString &title);
    bool showOrderRecordDialog(Common::Entities::SalesOrderRecord &record, const QString &title);

    SalesViewModel &m_viewModel;
    QTabWidget *m_tabs{nullptr};
    QLineEdit *m_ordersFilterField{nullptr};
    QPushButton *m_addOrderButton{nullptr};
    QPushButton *m_editOrderButton{nullptr};
    QPushButton *m_deleteOrderButton{nullptr};
    QTableWidget *m_ordersTable{nullptr};
    QPushButton *m_addRecordButton{nullptr};
    QPushButton *m_editRecordButton{nullptr};
    QPushButton *m_deleteRecordButton{nullptr};
    QTableWidget *m_orderRecordsTable{nullptr};
    QLabel *m_ordersStatusLabel{nullptr};

    QLineEdit *m_invoicesFilterField{nullptr};
    QPushButton *m_exportInvoiceButton{nullptr};
    QTableWidget *m_invoiceOrdersTable{nullptr};
    QTableWidget *m_invoiceRecordsTable{nullptr};
    QPlainTextEdit *m_invoicePreview{nullptr};
    QLabel *m_invoicesStatusLabel{nullptr};

    QVector<Common::Entities::SaleOrder> m_allOrders;
    QVector<Common::Entities::SalesOrderRecord> m_currentRecords;
    RecordsContext m_recordsContext{RecordsContext::None};
};
