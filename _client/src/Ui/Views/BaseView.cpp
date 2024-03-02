#include "BaseView.hpp"
#include "Ui/Views/ViewStyles.hpp"

BaseView::BaseView(QWidget *parent) : QWidget(parent)
{
    setupUi(); // Call function to set up the user interface
}

BaseView::~BaseView() {}

void BaseView::setupUi()
{
    // Set up the main layout
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

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
    m_buttonRow = createBoxLayout<QHBoxLayout>({m_addButton, m_deleteButton, m_editButton});
    m_buttonRow->addSpacerItem(m_buttonRowSpacerItem);

    // Add the button layout to the main layout
    m_mainLayout->addLayout(m_buttonRow);

    // Create a data table and add it to the main layout
    m_dataTable = new QTableWidget(this);
    m_mainLayout->addWidget(m_dataTable);

    // Create a status label and spacer item for additional info
    m_status = new QLabel("Status: ready");
    m_additionalInfoSpacerItem = new QSpacerItem(
        ViewsStyles::ButtonsSpacer::WIDTH,
        ViewsStyles::ButtonsSpacer::HEIGHT,
        QSizePolicy::Expanding,
        QSizePolicy::Fixed);

    // Create additional info layout with m_status
    m_additionalInfo = createBoxLayout<QHBoxLayout>({m_status});
    m_additionalInfo->addSpacerItem(m_additionalInfoSpacerItem);

    // Add the additional information layout to the main layout
    m_mainLayout->addLayout(m_additionalInfo);
}

// Function to create layout with existed widgets
template<typename Layout, class>
Layout* BaseView::createBoxLayout(QVector<QWidget*> widgets, QWidget* parent) {
    if (true) {
        Layout* layout = new Layout(parent);
        for (auto widget : widgets)
            layout->addWidget(widget);
        return layout;
    }
    return nullptr;
}
