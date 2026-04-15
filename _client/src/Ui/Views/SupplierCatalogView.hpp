#pragma once

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QVector>
#include <QWidget>

#include "common/Entities/SuppliersProductInfo.hpp"

class SupplierCatalogViewModel;

class SupplierCatalogView : public QWidget {
    Q_OBJECT

public:
    explicit SupplierCatalogView(SupplierCatalogViewModel &viewModel, QWidget *parent = nullptr);

signals:
    void errorOccurred(const QString &message);

private slots:
    void onSupplierProductsChanged();
    void onAddClicked();
    void onDeleteClicked();
    void onItemChanged(QTableWidgetItem *item);
    void onImportCsvClicked();
    void onFilterChanged(const QString &text);
    void onSelectionChanged();

private:
    void setupUi();
    void applyFilter();
    void updateActionButtons();
    void updateStatus(int visibleCount, int totalCount);
    void persistRow(int row);
    void fillTable(const QVector<Common::Entities::SuppliersProductInfo> &supplierProducts);
    bool importCsvFile(const QString &filePath);

    SupplierCatalogViewModel &m_viewModel;
    QLineEdit *m_filterField{nullptr};
    QPushButton *m_addButton{nullptr};
    QPushButton *m_deleteButton{nullptr};
    QPushButton *m_importButton{nullptr};
    QTableWidget *m_table{nullptr};
    QLabel *m_statusLabel{nullptr};
    QVector<Common::Entities::SuppliersProductInfo> m_allSupplierProducts;
    bool m_isSyncingTable{false};
};
