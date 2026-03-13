#pragma once

#include <QPointer>
#include <QObject>

#include <functional>

class DialogManager;
class MainWindow;

class StartupController : public QObject {
    Q_OBJECT

public:
    using MainWindowFactory = std::function<MainWindow *()>;

    explicit StartupController(DialogManager &dialogManager, MainWindowFactory mainWindowFactory,
                               QObject *parent = nullptr);

    void start();
    MainWindow *mainWindow() const;

private slots:
    void onLoginSucceeded();

private:
    DialogManager &m_dialogManager;
    MainWindowFactory m_mainWindowFactory;
    QPointer<MainWindow> m_mainWindow;
};
