#pragma once

#include <QObject>
#include <QFont>
#include <QString>
#include <QMap>

// Networking
class ApiManager;

// UI
class MainWindow;

// Dialogs
class DialogManager;
class LoginDialog;
class RegistrationDialog;

// ViewModels
class PurchaseOrdersViewModel;
class LoginDialogModel;
class RegistrationDialogModel;

// Views
class MainWindow;
class AuthorizationView;
class RegistrationView;
class MainMenuViews;

/**
 * @class UiManager
 * @brief Manages UI components.
 *
 * This class is responsible for initializing and managing UI components.
 */
class UiManager : public QObject
{
    Q_OBJECT

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

    /**
     * @brief Construct a new UiManager object.
     *
     * Initializes a UiManager object. Optionally, a parent QObject can be passed.
     *
     * @param parent Parent QObject (default is nullptr).
     */
    explicit UiManager(ApiManager *apiClient, QObject *parent = nullptr) noexcept;

    /**
     * @brief Destroy the UiManager object.
     *
     * Destroys the UiManager object and cleans up its resources.
     */
    ~UiManager() noexcept override;

    /**
     * @brief Shows the Authorization dialog
     */
    void startUiProcess();

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

public: // getters for ViewModels
signals:
    // ---- TO UI ----
    /**
     * @brief Signal emitted when the theme changes.
     *
     * @param newTheme New theme as a Theme enum value.
     */
    void themeChanged(Theme newTheme);

public slots:
    /**
     * @brief Displays an error message box.
     *
     * Shows a modal message box with an error message.
     *
     * @param message The error message to display.
     */
    void handleError(const QString &message);

private:
    /**
     * @brief Initialize all UI components.
     *
     * This method is responsible for setting up UI components.
     */
    void init();

    /**
     * @brief Initialize the theme.
     *
     * Sets the initial theme based on some criteria or settings.
     */
    void initTheme();

    /**
     * @brief Initialize UI viewModels.
     *
     * Sets up various UI viewModels required by the application.
     */
    void initViewModels();

    /**
     * @brief Initialize UI views.
     *
     * Sets up various UI views required by the application.
     */
    void initViews();

    /**
     * @brief Setup API networking connections
     */
    void setupApiConnections();

    /**
     * @brief Setup Models-Views-ViewModels connections according to MVVM
     */
    void setupMVVMConnections();

private:
    ApiManager *m_apiManager;

    // UI
    MainWindow *m_mainWindow;

    // Dialogs
    DialogManager *m_dialogManager;
    LoginDialog *m_loginDialog;
    RegistrationDialog *m_registrationDialog;

    // ViewModels
    PurchaseOrdersViewModel *m_purchaseOrdersViewModel;

    // Views

    // Styles
    Theme m_theme; ///< Member variable storing the current themes
};
