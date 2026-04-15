#include "BaseView.hpp"
#include "Ui/Views/TableInteractionHelpers.hpp"

#include <QAbstractItemView>
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

void BaseView::enableInlineEditing()
{
    configureEditableTable(m_dataTable, QStringLiteral("baseTableInlineEditor"));
}

void BaseView::setupUi()
{
    SPDLOG_TRACE("BaseView::setupUi");
    // Create buttons
    m_addButton    = new QPushButton(tr("Add"), this);
    m_deleteButton = new QPushButton(tr("Delete"), this);
    m_addButton->setObjectName("baseAddButton");
    m_deleteButton->setObjectName("baseDeleteButton");

    // Create spacer item for the button layout
    m_buttonRowSpacerItem =
        new QSpacerItem(UiScale::scalePx(ViewsStyles::ButtonsSpacer::WIDTH),
                        UiScale::scalePx(ViewsStyles::ButtonsSpacer::HEIGHT),
                        QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Create a row layout for buttons and add spacer item
    m_buttonRow = Utils::createLayout<QHBoxLayout>(
        {m_addButton, m_deleteButton, m_buttonRowSpacerItem});

    // Create a data table and add it to the main layout
    m_dataTable = new QTableWidget(this);
    m_dataTable->setObjectName("baseDataTable");
    m_dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTable->setAlternatingRowColors(true);
    m_dataTable->setShowGrid(false);
    m_dataTable->verticalHeader()->setVisible(false);
    configureTableViewportMetrics(m_dataTable);

    // Create a status label and spacer item for additional info
    m_status = new QLabel(tr("Status: ready"), this);
    m_status->setObjectName("baseStatusLabel");
    m_status->setProperty("muted", true);
    m_additionalInfoSpacerItem =
        new QSpacerItem(UiScale::scalePx(ViewsStyles::ButtonsSpacer::WIDTH),
                        UiScale::scalePx(ViewsStyles::ButtonsSpacer::HEIGHT),
                        QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Create additional info layout with m_status and additionalInfoSpacerItem
    m_additionalInfo = Utils::createLayout<QHBoxLayout>({m_status, m_additionalInfoSpacerItem});

    // Set up the main layout
    m_mainLayout =
        Utils::createLayout<QVBoxLayout>({m_buttonRow, m_dataTable, m_additionalInfo}, this);
    m_mainLayout->setSpacing(UiScale::scalePx(12));
    m_mainLayout->setContentsMargins(UiScale::scalePx(12), UiScale::scalePx(12),
                                     UiScale::scalePx(12), UiScale::scalePx(12));
    this->setLayout(m_mainLayout);
}
