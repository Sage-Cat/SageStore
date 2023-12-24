#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QStatusBar>
#include "Ui/WidgetStyles.hpp"
#include "Component/BaseWidget.hpp"

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

    // colomn layout
    QHBoxLayout *colomnLayout;

    // status bar
    QStatusBar *statusBar;

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;
};
