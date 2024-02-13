#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>

#include "Views/Actions.hpp"
#include "Views/ViewStyles.hpp"

class MainWindow : public QMainWindow 
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // setup window
    void init();
    // setup UI
    void setupUI();

    void setupMenu();

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;

    QMenu* createModuleMenu(const QString& menuTitle, const std::vector<ActionTypes>& actions);

private:
    // stack view
    QStackedWidget *m_stackView;

    // status bar
    QStatusBar *m_statusBar;

    // menu bar
    QMenuBar *m_mainMenuBar;
};
