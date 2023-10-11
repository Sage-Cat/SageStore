#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <QObject>
#include <QQmlApplicationEngine>

#include <memory>

class UIManager : public QObject
{
    Q_OBJECT

public:
    explicit UIManager(QObject *parent = nullptr);
    ~UIManager() override;

    // UIManager initialization
    void init();

signals:
    // Signals that can be emitted by UIManager
    void uiEventOccurred(int eventCode);
    void errorOccurred(const QString &errorMessage);

public slots:
    // Slots to respond to changes or events from ApplicationLogic or other classes
    void onModuleEvent(int eventCode);

private:
    void initModules();
    void initDialogues();

private:
    std::unique_ptr<QQmlApplicationEngine> m_engine;
};

#endif // UIMANAGER_HPP
