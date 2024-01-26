#pragma once

#include <QObject>

#include "IViewModel.hpp"

/**
 * @brief Manages the UI rendering and interactions for the purchase View.
 *
 * @details This class represents the view for purchases.
 *          It encapsulates the behavior and data necessary to interact with the
 *          purchase View within the UI.
 */
class PurchaseOrdersViewModel : public QObject, public IViewModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new PurchaseOrdersViewModel object.
     *
     * @param parent Parent QObject. Default is nullptr.
     */
    explicit PurchaseOrdersViewModel(QObject *parent = nullptr);

    /**
     * @brief Destroys the PurchaseOrdersViewModel object and cleans up resources.
     */
    ~PurchaseOrdersViewModel() override;
};
