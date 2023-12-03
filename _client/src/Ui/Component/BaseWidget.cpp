#include "BaseWidget.hpp"

BaseWidget::BaseWidget(QWidget *parent = nullptr)
{
    setupUi();
    setupConnections();
}

void BaseWidget::setupUi()
{
    // set main layout
    this->setLayout(_mMainLayout);

    //  add buttons to button layout
    _mButtonRow->addWidget(_mAddButton);
    _mButtonRow->addWidget(_mEditButton);
    _mButtonRow->addWidget(_mDeleteButton);

    // add spacer to button layout
    _mButtonRow->addSpacerItem(_mAdditionalButtonSpace);

    // add button layout to main layout
    _mMainLayout->addLayout(_mButtonRow);

    // add data table to main layout
    _mMainLayout->addWidget(_mDataTable);

    // add label to additional info
    _mAdditionalInfo->addWidget(_mStatus);

    // add additional info in main layout
    _mMainLayout->addLayout(_mAdditionalInfo);