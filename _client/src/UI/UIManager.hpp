#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QFont>

class PurchaseView;

/**
 * @class UiManager
 * @brief Manages UI components.
 *
 * This class is responsible for initializing and managing UI components.
 */
class UiManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    /**
     * @enum Theme
     * @brief Identifies the set themes.
     */
    enum class Theme : int
    {
        Dark = 0,
        Light
    };
    Q_ENUM(Theme)

    /**
     * @brief Construct a new UiManager object.
     *
     * Initializes a UiManager object. Optionally, a parent QObject can be passed.
     *
     * @param parent Parent QObject (default is nullptr).
     */
    explicit UiManager(QObject *parent = nullptr) noexcept;

    /**
     * @brief Destroy the UiManager object.
     *
     * Destroys the UiManager object and cleans up its resources.
     */
    ~UiManager() noexcept override;

    /**
     * @brief Initialize UI components.
     *
     * This method is responsible for setting up UI components.
     */
    void initUi();

    /**
     * @brief Get the current theme.
     *
     * @return Current theme as a Theme enum value.
     */
    Theme theme() const;

    /**
     * @brief Set the theme.
     *
     * Updates the current theme.
     *
     * @param theme New theme as a Theme enum value.
     */
    void setTheme(Theme theme);

    /**
     * @brief Get the default font.
     *
     * @return Default font as a QFont object.
     */
    QFont defaultFont() const;

signals:
    // ---- TO UI ----
    /**
     * @brief Signal emitted when the theme changes.
     *
     * @param newTheme New theme as a Theme enum value.
     */
    void themeChanged(Theme newTheme);

    /**
     * @brief request to push purchase view in stack
     *
     */
    void pushToStackRequested(QQuickItem *item);

    // ---- FROM UI ----

private:
    /**
     * @brief Initialize the theme.
     *
     * Sets the initial theme based on some criteria or settings.
     */
    void initTheme();

    /**
     * @brief Initialize the main window.
     *
     * Sets up the main application window.
     */
    void initMainWindow();

    /**
     * @brief Initialize UI modules.
     *
     * Sets up various UI modules required by the application.
     */
    void initModules();

    /**
     * @brief Initialize UI dialogues.
     *
     * Sets up any dialogues used in the UI.
     */
    void initDialogues();

    /**
     * @brief Pushes a QML item onto the main stack view.
     *
     *
     * @param item The QObject derived item (e.g., a QML Component)
     *             to be pushed onto the StackView.
     *
     * @return bool if tab is added successfully
     */
    bool addToStack(QQmlComponent *component);

    /**
     * @brief Adds a new tab to the main window for the PurchaseView.
     *
     * @note This function requires that the QML `StackView` named "StackView" supports
     * the addition of new tabs programmatically (e.g., has an "addTab" method or equivalent).
     */
    Q_INVOKABLE void showPurchaseOrdersView();

private:
    Theme m_theme;                  ///< Member variable storing the current theme.
    QQmlApplicationEngine m_engine; ///< QQmlApplicationEngine for running the QML engine.

    // Views
    PurchaseView *m_purchaseView; ///< Pointer to the purchase view.
};
