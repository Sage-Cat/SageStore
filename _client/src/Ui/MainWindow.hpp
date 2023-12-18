#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // stack view
    QStackedWidget *stackView;

    // colomn layout
    QHBoxLayout *colomnLayout;

    // override resizeEvent
    void resizeEvent(QResizeEvent *event) override;
};
