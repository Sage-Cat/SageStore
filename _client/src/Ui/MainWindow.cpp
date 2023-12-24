#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupWindow();
}

void MainWindow::setupWindow()
{
    // set size
    this->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // create colomn layout
    colomnLayout = new QHBoxLayout(this);

    // add stackView
    stackView = new QStackedWidget;
    userRoles = new BaseWidget;
    stackView->addWidget(userRoles);

    colomnLayout->addWidget(stackView);

    // add status bar
    statusBar = new QStatusBar;
    colomnLayout->addWidget(statusBar);

    this->setLayout(colomnLayout);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
}
