#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>

#include "Views/ViewStyles.hpp"

class MainWindowView : public QMainWindow
{
    Q_OBJECT

public:
    MainWindowView(QWidget *parent = nullptr);
    ~MainWindowView();

protected:
    // setup window
    void init();
    // setup UI
    void setupUI();

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;

private:
    // stack view
    QStackedWidget *m_stackView;

    // status bar
    QStatusBar *m_statusBar;
};
