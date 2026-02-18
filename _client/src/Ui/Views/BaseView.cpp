#include "BaseView.hpp"
#include <QHeaderView>

#include "Ui/UiScale.hpp"
#include "Ui/Views/ViewStyles.hpp"

#include "Ui/ViewModels/BaseViewModel.hpp"

#include "common/SpdlogConfig.hpp"

BaseView::BaseView(BaseViewModel &viewModel, QWidget *parent) : QWidget(parent)
{
    setupUi();
    connect(this, &BaseView::errorOccurred, &viewModel, &BaseViewModel::errorOccurred);
}

BaseView::~BaseView() {}

void BaseView::setupUi()
{
    SPDLOG_TRACE("BaseView::setupUi");
    // Create buttons
    m_addButton    = new QPushButton("Add", this);
    m_deleteButton = new QPushButton("Delete", this);
    m_editButton   = new QPushButton("Edit", this);

    // Create spacer item for the button layout
    m_buttonRowSpacerItem =
        new QSpacerItem(UiScale::scalePx(ViewsStyles::ButtonsSpacer::WIDTH),
                        UiScale::scalePx(ViewsStyles::ButtonsSpacer::HEIGHT),
                        QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Create a row layout for buttons and add spacer item
    m_buttonRow = Utils::createLayout<QHBoxLayout>(
        {m_addButton, m_deleteButton, m_editButton, m_buttonRowSpacerItem});

    // Create a data table and add it to the main layout
    m_dataTable = new QTableWidget(this);
    m_dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Create a status label and spacer item for additional info
    m_status = new QLabel("Status: ready", this);
    m_additionalInfoSpacerItem =
        new QSpacerItem(UiScale::scalePx(ViewsStyles::ButtonsSpacer::WIDTH),
                        UiScale::scalePx(ViewsStyles::ButtonsSpacer::HEIGHT),
                        QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Create additional info layout with m_status and additionalInfoSpacerItem
    m_additionalInfo = Utils::createLayout<QHBoxLayout>({m_status, m_additionalInfoSpacerItem});

    // Set up the main layout
    m_mainLayout =
        Utils::createLayout<QVBoxLayout>({m_buttonRow, m_dataTable, m_additionalInfo}, this);
    this->setLayout(m_mainLayout);
}
