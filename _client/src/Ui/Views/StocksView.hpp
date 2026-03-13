#pragma once

#include <QLineEdit>
#include <QVector>

#include "Ui/DisplayData/InventoryStock.hpp"
#include "Ui/Views/BaseView.hpp"

class StocksViewModel;

class StocksView : public BaseView {
    Q_OBJECT

public:
    explicit StocksView(StocksViewModel &viewModel, QWidget *parent = nullptr);

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onEditButtonClicked();
    void onFilterTextChanged(const QString &text);
    void onStocksChanged();
    void onSelectionChanged();

private:
    void applyFilter();
    void fillTable(const QVector<DisplayData::InventoryStock> &stocks);
    void updateStatus(int visibleCount, int totalCount);
    void updateActionButtons();
    bool showStockDialog(DisplayData::InventoryStock &stock, const QString &title);

    StocksViewModel &m_viewModel;
    QLineEdit *m_filterField{nullptr};
    QVector<DisplayData::InventoryStock> m_allStocks;
};
