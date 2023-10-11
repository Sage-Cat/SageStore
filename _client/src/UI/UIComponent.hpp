/**
 * @file UiComponent.hpp
 * @brief Header file for the UiComponent interface.
 */

#ifndef UICOMPONENT_HPP
#define UICOMPONENT_HPP

/**
 * @interface UiComponent
 * @brief Interface for UI components.
 *
 * This interface defines the basic functionalities for rendering UI components.
 */
class UiComponent
{
public:
    /**
     * @brief Virtual destructor for UiComponent.
     *
     * Ensures proper cleanup for derived classes.
     */
    virtual ~UiComponent() = default;

    /**
     * @brief Renders the UI component.
     *
     * This pure virtual function should be implemented by derived classes to handle their rendering logic.
     */
    virtual void render() = 0;
};

#endif // UICOMPONENT_HPP
