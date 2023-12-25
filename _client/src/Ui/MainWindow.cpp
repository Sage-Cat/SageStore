#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupWindow();
    SPDLOG_DEBUG("Finish window setup");
}

void MainWindow::setupWindow()
{
    // set size
    this->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // add stackView
    stackView = new QStackedWidget(this);
    stackView->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // stackView pages
    SPDLOG_DEBUG("Starting userRoles init");
    userRoles = new BaseWidget(this);
    stackView->addWidget(userRoles);
    SPDLOG_DEBUG("Finish userRoles init");

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
    stackView->resize(this->size().width(), this->size().height());
}
