#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQmlComponent>

#include "Ui/UiComponent.hpp"

/**
 * @brief Manages the UI rendering and interactions for the purchase View.
 *
 * @details This class represents the view for purchases.
 *          It encapsulates the behavior and data necessary to interact with the
 *          purchase View within the UI.
 */
class PurchaseView : public QObject, public UiComponent
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new PurchaseView object.
     *
     * @param engine Pointer to the main QML engine.
     * @param parent Parent QObject. Default is nullptr.
     */
    explicit PurchaseView(QQmlApplicationEngine *engine, QObject *parent = nullptr);

    /**
     * @brief Destroys the PurchaseView object and cleans up resources.
     */
    ~PurchaseView() override;

    /**
     * @brief Renders the purchase View.
     *
     * @details If the QML component is ready, this function will render
     *          the purchase View. If the View instance does not exist,
     *          it will be created.
     */
    void render() override;

    /**
     * @brief Get the QQmlComponent associated with this object.
     *
     * This function returns a pointer to the QQmlComponent that is associated with this object.
     *
     * @return A pointer to the QQmlComponent object.
     */
    QQmlComponent *component() const;

private:
    QQmlApplicationEngine *m_engine; ///< Pointer to the main QML engine.
    QQmlComponent *m_component;      ///< Component for the purchaseView.
    QObject *m_purchaseViewInstance; ///< Instance of the rendered purchaseView.
};
