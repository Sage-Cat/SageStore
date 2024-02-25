#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>

#include "Ui/Views/Actions.hpp"
#include "Ui/Views/ViewStyles.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

    void init();

protected:
    void setupUi();
    void setupMenu();

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;

    QMenu *createModuleMenu(const QString &menuTitle, const std::vector<Actions::Types> &names);

private:
    // stack view
    QStackedWidget *m_stackView;

    // status bar
    QStatusBar *m_statusBar;

    // menu bar
    QMenuBar *m_mainMenuBar;
};
