#include "MainWindow.hpp"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    init();
    setupUI();
}

void MainWindow::init()
{
    // add stackView
    m_stackView = new QStackedWidget(this);
    m_stackView->resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // add status bar
    m_statusBar = new QStatusBar(this);
    this->setStatusBar(m_statusBar);
}

void MainWindow::setupUI()
{
    // set size and at fullscreen
    this->resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    this->showMaximized();
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_stackView->resize(this->size().width(), this->size().height());
}
