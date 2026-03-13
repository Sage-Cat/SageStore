#pragma once

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
    void onProductTypesChanged();

private:
    void fillTable(const QVector<DisplayData::ProductType> &productTypes);
    bool showProductTypeDialog(DisplayData::ProductType &productType, const QString &title);

    ProductTypesViewModel &m_viewModel;
};
