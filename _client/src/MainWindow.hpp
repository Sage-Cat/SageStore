#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QStatusBar>

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

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;

private:
    // stack view
    QStackedWidget *m_stackView;

    // status bar
    QStatusBar *m_statusBar;
};
