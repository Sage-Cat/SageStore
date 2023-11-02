#pragma once

#include <QObject>
#include <QQmlApplicationEngine>

#include "UiViewModel.hpp"

/**
 * @brief Manages the UI rendering and interactions for the purchase View.
 *
 * @details This class represents the view for purchases.
 *          It encapsulates the behavior and data necessary to interact with the
 *          purchase View within the UI.
 */
class NAME_HEREViewModel : public QObject, public UiViewModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new NAME_HEREViewModel object.
     *
     * @param parent Parent QObject. Default is nullptr.
     */
    explicit NAME_HEREViewModel(QObject *parent = nullptr);

    /**
     * @brief Destroys the NAME_HEREViewModel object and cleans up resources.
     */
    ~NAME_HEREViewModel() override;
};
