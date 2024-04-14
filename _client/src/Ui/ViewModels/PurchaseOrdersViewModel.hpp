#pragma once

#include <QObject>

#include "Ui/ViewModels/IViewModel.hpp"

/**
 * @brief Manages the UI rendering and interactions for the purchase View.
 */
class PurchaseOrdersViewModel : public QObject, public IViewModel
{
    Q_OBJECT

public:
    explicit PurchaseOrdersViewModel(QObject *parent = nullptr);
    ~PurchaseOrdersViewModel() override;
};
