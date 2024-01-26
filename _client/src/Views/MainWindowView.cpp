#include "MainWindowView.hpp"

MainWindowView::MainWindowView(QWidget *parent)
    : QMainWindow(parent)
{
    init();
    setupUI();
}

void MainWindowView::init()
{
    // add stackView
    m_stackView = new QStackedWidget(this);
    m_stackView->resize(MainWindowSize::WINDOW_WIDTH, MainWindowSize::WINDOW_HEIGHT);

    // add status bar
    m_statusBar = new QStatusBar(this);
    this->setStatusBar(m_statusBar);
}

void MainWindowView::setupUI()
{
    // set size and at fullscreen
    this->resize(MainWindowSize::WINDOW_WIDTH, MainWindowSize::WINDOW_HEIGHT);
    this->showMaximized();
}

MainWindowView::~MainWindowView()
{
}

void MainWindowView::resizeEvent(QResizeEvent *event)
{
    m_stackView->resize(this->size().width(), this->size().height());
}
