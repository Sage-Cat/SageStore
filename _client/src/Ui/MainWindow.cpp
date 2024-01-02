#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    init();
}

void MainWindow::init()
{
    // set size
    this->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // add stackView
    m_stackView = new QStackedWidget(this);
    m_stackView->resize(MainWidSize::WINDOW_WIDTH, MainWidSize::WINDOW_HEIGHT);

    // stackView pages
    // m_userRoles = new BaseWidget(this);
    // m_stackView->addWidget(userRoles);

    // add status bar
    m_statusBar = new QStatusBar(this);
    this->setStatusBar(m_statusBar);
}

MainWindow::~MainWindow()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    stackView->resize(this->size().width(), this->size().height());
}
