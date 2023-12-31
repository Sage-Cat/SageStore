#pragma once

/**
 * @interface UiViewModel
 * @brief Interface for UI components.
 *
 * This interface defines the basic functionalities for rendering UI components.
 */
class UiViewModel
{
public:
    /**
     * @brief Virtual destructor for UiViewModel.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~UiViewModel() = default;
};
