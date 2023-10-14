#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QFont>

/**
 * @class UIManager
 * @brief Manages UI components.
 *
 * This class is responsible for initializing and managing UI components.
 * It also provides various signals and slots for UI interaction.
 */
class UIManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged) ///< Property for the current theme.

public:
    /**
     * @enum Theme
     * @brief Identifies the set themes.
     */
    enum class Theme : int
    {
        Dark = 0, ///< Dark theme
        Light     ///< Light theme
    };
    Q_ENUM(Theme)

    /**
     * @brief Construct a new UIManager object.
     *
     * Initializes a UIManager object. Optionally, a parent QObject can be passed.
     *
     * @param parent Parent QObject (default is nullptr).
     */
    explicit UIManager(QObject *parent = nullptr) noexcept;

    /**
     * @brief Destructor.
     *
     * Destroys the UIManager object and cleans up its resources.
     */
    ~UIManager() noexcept override;

    /**
     * @brief Initialize UI components.
     *
     * This method is responsible for setting up UI components.
     */
    void init();

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
    /**
     * @brief Signal emitted when a UI event occurs.
     *
     * @param eventCode The code representing the UI event.
     */
    void uiEventOccurred(int eventCode);

    /**
     * @brief Signal emitted when a UI error occurs.
     *
     * @param errorMessage The message describing the error.
     */
    void errorOccurred(QString errorMessage);

    /**
     * @brief Signal emitted when the theme changes.
     *
     * @param newTheme New theme as a Theme enum value.
     */
    void themeChanged(Theme newTheme);

public slots:
    /**
     * @brief Slot for handling module events.
     *
     * @param eventCode Code representing the module event.
     */
    void onModuleEvent(int eventCode);

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

private:
    Theme m_theme;                  ///< Member variable storing the current theme.
    QQmlApplicationEngine m_engine; ///< QQmlApplicationEngine for running the QML engine.
};
