#pragma once

/**
 * @interface IViewModel
 * @brief Interface for UI components.
 *
 * This interface defines the basic functionalities for rendering UI components.
 */
class IViewModel
{
public:
    /**
     * @brief Virtual destructor for IViewModel.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~IViewModel() = default;
};
