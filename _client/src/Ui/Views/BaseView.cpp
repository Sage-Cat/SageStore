#include "BaseView.hpp"
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

void BaseView::setupUi()
{
    SPDLOG_TRACE("BaseView::setupUi");
    // Create buttons
    m_addButton    = new QPushButton("Add", this);
    m_deleteButton = new QPushButton("Delete", this);
    m_editButton   = new QPushButton("Edit", this);
    m_addButton->setObjectName("baseAddButton");
    m_deleteButton->setObjectName("baseDeleteButton");
    m_editButton->setObjectName("baseEditButton");

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
    m_dataTable->setObjectName("baseDataTable");
    m_dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_dataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_dataTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_dataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_dataTable->setAlternatingRowColors(true);
    m_dataTable->setShowGrid(false);
    m_dataTable->verticalHeader()->setVisible(false);
    m_dataTable->setStyleSheet(
        "QTableWidget {"
        " background-color: #fbfbfd;"
        " alternate-background-color: #f1f4f8;"
        " border: 1px solid #d7dde7;"
        " border-radius: 8px;"
        " gridline-color: #d7dde7;"
        " selection-background-color: #dbeafe;"
        " selection-color: #0f172a;"
        "}"
        "QHeaderView::section {"
        " background-color: #e8eef6;"
        " color: #0f172a;"
        " padding: 6px;"
        " border: 0;"
        " border-bottom: 1px solid #d7dde7;"
        " font-weight: 600;"
        "}");

    // Create a status label and spacer item for additional info
    m_status = new QLabel("Status: ready", this);
    m_status->setObjectName("baseStatusLabel");
    m_status->setStyleSheet("color: #475569; font-weight: 500;");
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
    setStyleSheet(
        "QPushButton {"
        " background-color: #1d4ed8;"
        " color: white;"
        " border: 0;"
        " border-radius: 6px;"
        " padding: 6px 12px;"
        "}"
        "QPushButton#baseDeleteButton {"
        " background-color: #b91c1c;"
        "}"
        "QPushButton:hover {"
        " background-color: #1e40af;"
        "}"
        "QPushButton#baseDeleteButton:hover {"
        " background-color: #991b1b;"
        "}"
        "QLineEdit, QPlainTextEdit {"
        " border: 1px solid #cbd5e1;"
        " border-radius: 6px;"
        " padding: 6px;"
        " background-color: white;"
        "}");
    this->setLayout(m_mainLayout);
}
