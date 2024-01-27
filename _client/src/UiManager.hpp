#pragma once

#include <QObject>
#include <QFont>
#include <QString>
#include <QMap>

// ViewModels
class PurchaseOrdersViewModel;
class AuthorizationDialogModel;
class RegistrationDialogModel;

// Views
class MainWindow;
class AuthorizationDialog;
class RegistrationDialog;

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
    explicit UiManager(QObject *parent = nullptr) noexcept;

    /**
     * @brief Destroy the UiManager object.
     *
     * Destroys the UiManager object and cleans up its resources.
     */
    ~UiManager() noexcept override;

    /**
     * @brief Shows the Authorization dialog
     */
    void initiateAuthorizationProcess();

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
    /**
     * @brief Get Authorization ViewModel
     *
     * @return m_authorizationViewModel
     */
    AuthorizationDialogModel *authorizationViewModel() const;

    /**
     * @brief Get Registration ViewModel
     *
     * @return m_registrationViewModel
     */
    RegistrationDialogModel *registrationViewModel() const;

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
    void showErrorMessageBox(const QString &message);

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
     * @brief Initialize the main window.
     *
     * Sets up the main application window.
     */
    void initMainWindow();

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
     * @brief Setup viewModels - view connections according to MVVM
     */
    void setupVVMConnections();

private:
    Theme m_theme; ///< Member variable storing the current theme.

    MainWindow *m_mainWindow;
    // ViewModels
    AuthorizationDialogModel *m_authorizationViewModel;
    RegistrationDialogModel *m_registrationViewModel;
    PurchaseOrdersViewModel *m_purchaseOrdersViewModel;

    // Views
    AuthorizationDialog *m_authorizationView;
    RegistrationDialog *m_registrationView;
};
