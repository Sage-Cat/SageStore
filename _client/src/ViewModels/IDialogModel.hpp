#pragma once

/**
 * @interface IDialogModel
 * @brief Interface for UI components.
 *
 * This interface defines the basic functionalities for rendering UI components.
 */
class IDialogModel
{
public:
    /**
     * @brief Virtual destructor for IDialogModel.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~IDialogModel() = default;
};
