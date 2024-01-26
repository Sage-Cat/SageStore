#include "BaseView.hpp"
#include "Views/ViewStyles.hpp"

BaseView::BaseView(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

BaseView::~BaseView() {}

void BaseView::setupUi()
{
    // set main layout
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    //  add buttons to button layout
    m_buttonRow = new QHBoxLayout;
    m_addButton = new QPushButton("Add", this);
    m_deleteButton = new QPushButton("Delete", this);
    m_editButton = new QPushButton("Edit", this);

    m_buttonRow->addWidget(m_addButton);
    m_buttonRow->addWidget(m_editButton);
    m_buttonRow->addWidget(m_deleteButton);

    // add spacer to button layout
    m_additionalButtonSpace = new QSpacerItem(SpaceSize::ADDITITIONAL_SPACE_WIDTH, SpaceSize::ADDITITIONAL_SPACE_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_buttonRow->addSpacerItem(m_additionalButtonSpace);

    // add button layout to main layout
    m_mainLayout->addLayout(m_buttonRow);

    // add data table to main layout
    m_dataTable = new QTableWidget(this);
    m_mainLayout->addWidget(m_dataTable);

    // add label to additional info
    m_status = new QLabel("Status: Ready", this);
    m_additionalInfo = new QHBoxLayout;
    m_additionalInfo->addWidget(m_status);

    // add additional space under table in main layout
    m_additionalUnderTableSpace = new QSpacerItem(SpaceSize::ADDITITIONAL_SPACE_WIDTH, SpaceSize::ADDITIONAL_UNDER_TABLE_SPACE_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_mainLayout->addSpacerItem(m_additionalUnderTableSpace);

    // add additional info in main layout
    m_mainLayout->addLayout(m_additionalInfo);

    // add additional space under unfo

    m_additionalUnderStatusSpace = new QSpacerItem(SpaceSize::ADDITITIONAL_SPACE_WIDTH, SpaceSize::ADDITIONAL_UNDER_STATUS_SPACE_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_mainLayout->addSpacerItem(m_additionalUnderStatusSpace);
}
