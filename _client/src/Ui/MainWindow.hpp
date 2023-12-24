#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>
#include "Ui/WidgetStyles.hpp"

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

    // status bar
    QStatusBar *statusBar;

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;
};
