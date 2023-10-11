/**
 * @file UIManager.hpp
 * @brief Header file for the UIManager class.
 */

#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <QObject>
#include <QQmlApplicationEngine>
#include <memory>

/**
 * @class UIManager
 * @brief Class responsible for managing the UI.
 *
 * This class handles the initialization and management of the UI components.
 * It also provides signals and slots for UI events and errors.
 */
class UIManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for UIManager.
     *
     * @param parent Pointer to the parent QObject.
     */
    explicit UIManager(QObject *parent = nullptr);

    /**
     * @brief Destructor for UIManager.
     */
    ~UIManager() override;

    /**
     * @brief Initializes the UIManager.
     *
     * This function initializes the UI modules and dialogues.
     */
    void init();

signals:
    /**
     * @brief Signal emitted when a UI event occurs.
     *
     * @param eventCode The code representing the UI event.
     */
    void uiEventOccurred(int eventCode);

    /**
     * @brief Signal emitted when an error occurs.
     *
     * @param errorMessage The error message to be displayed.
     */
    void errorOccurred(const QString &errorMessage);

public slots:
    /**
     * @brief Slot to handle module events.
     *
     * @param eventCode The code representing the module event.
     */
    void onModuleEvent(int eventCode);

private:
    /**
     * @brief Initializes the UI modules.
     */
    void initModules();

    /**
     * @brief Initializes the UI dialogues.
     */
    void initDialogues();

private:
    std::unique_ptr<QQmlApplicationEngine> m_engine; ///< Unique pointer to the QQmlApplicationEngine.
};

#endif // UIMANAGER_HPP
