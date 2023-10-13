#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QFont>

#include <memory>

/**
 * @class UIManager
 * @brief Manages UI components.
 *
 * Initializes and manages UI components. Provides signals and slots for UI interaction.
 */
class UIManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged)

public:
    /**
     * @brief Enum that identifies themes set
     */
    enum class Theme : int
    {
        Dark = 0,
        Light
    };
    Q_ENUM(Theme)

    /**
     * @brief Construct a new UIManager object.
     *
     * @param parent Parent QObject.
     */
    explicit UIManager(QObject *parent = nullptr) noexcept;

    /**
     * @brief Destroy the UIManager object.
     *
     */
    ~UIManager() noexcept override;

    /**
     * @brief Initialize UI components.
     *
     */
    void init();

    /**
     * @brief Gets the  current Theme
     *
     * @returns theme The value of enum Theme.
     * @see Theme
     */
    Theme theme() const;

    /**
     * @brief Set the Theme for the application.
     *
     * @param theme The value of enum Theme.
     * @see Theme
     */
    void setTheme(Theme theme);

    QFont defaultFont() const;

signals:
    /**
     * @brief Notify when a UI event occurs.
     *
     * @param eventCode The UI event code.
     */
    void uiEventOccurred(int eventCode);

    /**
     * @brief Notify when a UI error occurs.
     *
     * @param errorMessage The error message.
     */
    void errorOccurred(QString errorMessage);

    /**
     * @brief Notify QML about theme change.
     *
     * @param newTheme int value that represents Theme.
     * @see Theme
     */
    void themeChanged(Theme newTheme);

public slots:
    /**
     * @brief Handle module events.
     *
     * @param eventCode The module event code.
     */
    void onModuleEvent(int eventCode);

private:
    /**
     * @brief Initialize the theme.
     *
     */
    void initTheme();

    /**
     * @brief Initialize the main window.
     *
     */
    void initMainWindow();

    /**
     * @brief Initialize UI modules.
     *
     */
    void initModules();

    /**
     * @brief Initialize UI dialogues.
     *
     */
    void initDialogues();

private:
    Theme m_theme;
    QQmlApplicationEngine m_engine;
};
