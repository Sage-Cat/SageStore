#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include "Ui/WidgetStyles.hpp"
#include "Ui/Component/BaseWidget.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // setup window
    void init();

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;

private:
    // stack view
    QStackedWidget *m_stackView;
    BaseWidget *m_userRoles;

    // status bar
    QStatusBar *m_statusBar;
};
