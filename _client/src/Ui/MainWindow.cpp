#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // setupWindow();
    userRoles = new BaseWidget(this);
}

void MainWindow::setupWindow()
{
    // set size
    this->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // add stackView
    stackView = new QStackedWidget(this);
    stackView->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // stackView pages

    // stackView->addWidget(userRoles);

    // add status bar
    statusBar = new QStatusBar(this);
    statusBar->showMessage("test status bar");
    this->setStatusBar(statusBar);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
}
