#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupWindow();
}

void MainWindow::setupWindow()
{
    // set size
    this->setBaseSize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // create colomn layout
    colomnLayout = new QHBoxLayout(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
}
