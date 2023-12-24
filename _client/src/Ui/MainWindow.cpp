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

    // add stackView
    stackView = new QStackedWidget(this);
    stackView->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // stackView pages
    QTabWidget *testW = new QTabWidget(this);
    stackView->addWidget(testW);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
}
