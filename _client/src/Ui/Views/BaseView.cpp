#include "BaseView.hpp"
#include "Ui/Views/ViewStyles.hpp"

BaseView::BaseView(QWidget *parent) : QWidget(parent)
{
    setupUi(); // Call function to set up the user interface
}

BaseView::~BaseView() {
    //spacerItems
    delete m_buttonRowSpacerItem;
    delete m_additionalInfoSpacerItem;

    //layouts
    delete m_buttonRow;
    delete m_additionalInfo;
}

void BaseView::setupUi()
{
    // Create buttons
    m_addButton = new QPushButton("Add", this);
    m_deleteButton = new QPushButton("Delete", this);
    m_editButton = new QPushButton("Edit", this);

    // Create spacer item for the button layout
    m_buttonRowSpacerItem = new QSpacerItem(
        ViewsStyles::ButtonsSpacer::WIDTH,
        ViewsStyles::ButtonsSpacer::HEIGHT,
        QSizePolicy::Expanding,
        QSizePolicy::Fixed);

    // Create a row layout for buttons and add spacer item
    m_buttonRow = Utils::createLayout<QHBoxLayout>({m_addButton, m_deleteButton, m_editButton, m_buttonRowSpacerItem});

    // Create a data table and add it to the main layout
    m_dataTable = new QTableWidget(this);

    // Create a status label and spacer item for additional info
    m_status = new QLabel("Status: ready", this);
    m_additionalInfoSpacerItem = new QSpacerItem(
        ViewsStyles::ButtonsSpacer::WIDTH,
        ViewsStyles::ButtonsSpacer::HEIGHT,
        QSizePolicy::Expanding,
        QSizePolicy::Fixed);

    // Create additional info layout with m_status and additionalInfoSpacerItem
    m_additionalInfo = Utils::createLayout<QHBoxLayout>({m_status, m_additionalInfoSpacerItem});

    // Set up the main layout
    m_mainLayout = Utils::createLayout<QVBoxLayout>({m_buttonRow, m_dataTable, m_additionalInfo}, this);
    this->setLayout(m_mainLayout);
}
