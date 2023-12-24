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

private:
    // setup window
    void setupWindow();

    // stack view
    QStackedWidget *stackView;
    BaseWidget *userRoles;

    // status bar
    QStatusBar *statusBar;

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;
};
