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
class UserRolesViewModel : public QObject, public UiViewModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new UserRolesViewModel object.
     *
     * @param parent Parent QObject. Default is nullptr.
     */
    explicit UserRolesViewModel(QObject *parent = nullptr);

    /**
     * @brief Destroys the UserRolesViewModel object and cleans up resources.
     */
    ~UserRolesViewModel() override;
};
