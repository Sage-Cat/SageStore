#pragma once

#include <QLineEdit>
#include <QVector>

#include "Ui/Views/BaseView.hpp"

#include "Ui/DisplayData/ProductType.hpp"

class ProductTypesViewModel;

class ProductTypesView : public BaseView {
    Q_OBJECT

public:
    explicit ProductTypesView(ProductTypesViewModel &viewModel, QWidget *parent = nullptr);

private slots:
    void onAddButtonClicked();
    void onDeleteButtonClicked();
    void onEditButtonClicked();
    void onFilterTextChanged(const QString &text);
    void onProductTypesChanged();
    void onSelectionChanged();

private:
    void applyFilter();
    void fillTable(const QVector<DisplayData::ProductType> &productTypes);
    bool showProductTypeDialog(DisplayData::ProductType &productType, const QString &title);
    void updateStatus(int visibleCount, int totalCount);
    void updateActionButtons();

    ProductTypesViewModel &m_viewModel;
    QLineEdit *m_filterField{nullptr};
    QVector<DisplayData::ProductType> m_allProductTypes;
};
