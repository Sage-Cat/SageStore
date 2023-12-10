#include "BaseWidget.hpp"

BaseWidget::BaseWidget(QWidget *parent = nullptr)
{
    setupUi();
}

void BaseWidget::setupUi()
{
    // set main layout
    m_MainLayout = new QVBoxLayout(this);
    this->setLayout(m_MainLayout);

    //  add buttons to button layout
    m_ButtonRow = new QHBoxLayout;
    m_AddButton = new QPushButton("Add", this);
    m_DeleteButton = new QPushButton("Delete", this);
    m_EditButton = new QPushButton("Edit", this);

    m_ButtonRow->addWidget(m_AddButton);
    m_ButtonRow->addWidget(m_EditButton);
    m_ButtonRow->addWidget(m_DeleteButton);

    // add spacer to button layout
    m_AdditionalButtonSpace = new QSpacerItem(ADDIT_SPACE_WIDTH, ADDIT_SPACE_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_ButtonRow->addSpacerItem(m_AdditionalButtonSpace);

    // add button layout to main layout
    m_MainLayout->addLayout(m_ButtonRow);

    // add data table to main layout
    m_DataTable = new QTableWidget(this);
    m_MainLayout->addWidget(m_DataTable);

    // add label to additional info
    m_Status = new QLabel("Status: Ready", this);
    m_AdditionalInfo->addWidget(m_Status);

    // add additional info in main layout
    m_AdditionalUnderTableSpace = new QSpacerItem(ADDIT_SPACE_WIDTH, ADDIT_SPACE_HEIGHT, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_MainLayout->addLayout(m_AdditionalInfo);
}