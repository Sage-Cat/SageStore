#include "BaseView.hpp"
#include "Ui/Views/ViewStyles.hpp"

BaseView::BaseView(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

BaseView::~BaseView() {}

void BaseView::setupUi()
{
    // Set main layout
    m_mainLayout = new QVBoxLayout(this);
    this->setLayout(m_mainLayout);

    // Create and set up group box for buttons
    buttonGroupBox = new QGroupBox("Actions", this);
    m_mainLayout->addWidget(buttonGroupBox);

    // Set up layout for buttons inside the group box
    setButtonLayout(buttonGroupBox);

    // Add buttons to button layout
    m_addButton = new QPushButton("Add", this);
    m_deleteButton = new QPushButton("Delete", this);
    m_editButton = new QPushButton("Edit", this);

    addButton({m_addButton, m_deleteButton, m_editButton});

    // Add spacer to button layout
    addUnderSpace();

    // Add data table to main layout
    m_dataTable = new QTableWidget(this);
    m_mainLayout->addWidget(m_dataTable);

    // Add additional space under data table
    addUnderSpace();

    // Add group box for additional info
    infoGroupBox = new QGroupBox("Additional Info", this);
    m_mainLayout->addWidget(infoGroupBox);

    // Set up layout for additional info inside the group box
    m_additionalInfo = new QHBoxLayout(infoGroupBox);

    // Add label for status
    m_status = new QLabel("Status: Ready", this);
    m_additionalInfo->addWidget(m_status);

    // Add additional space under status label
    addUnderSpace();

    //addButton(new QPushButton("test",this));
    //setStatus("Status: tested");
}

    //Methods with buttons
void BaseView::addButton(QPushButton* buttonToAdd) { m_buttonRow->addWidget(buttonToAdd);}

void BaseView::addButton(const std::vector<QPushButton*> buttonsToAdd){
    foreach (auto& button, buttonsToAdd)
        addButton(button);
}
void BaseView::removeButton(QPushButton* buttonToRemove) { m_buttonRow->removeWidget(buttonToRemove);}
void BaseView::removeButton(const std::vector<QPushButton*> buttonsToRemove){
    foreach (auto& button, buttonsToRemove)
        removeButton(button);
}

void BaseView::setButtonLayout(QHBoxLayout* buttonlayout) { m_buttonRow = buttonlayout;}
void BaseView::setButtonLayout(QGroupBox *buttonGroupBox) { setButtonLayout(new QHBoxLayout(buttonGroupBox)); }
const QLayout* BaseView::getButtonLayout() {return m_buttonRow;}

    //Methods with dataTable
//void BaseView::setDataTable(QTableWidget* dataTable) { m_dataTable = dataTable;}
const QTableWidget* BaseView::getDataTable() {return m_dataTable;}

    //Methods with AdditionalInfo
void BaseView::setStatus(const char* status) { m_status->setText(status);}
QLabel* BaseView::getStatus() { return m_status;}

    //Methods with UnderSpace
void BaseView::addUnderSpace(){
    m_mainLayout->addSpacerItem(new QSpacerItem(
        ViewsStyles::UnderStatusbarSpacer::WIDTH,
        ViewsStyles::UnderStatusbarSpacer::HEIGHT,
        QSizePolicy::Expanding,
        QSizePolicy::Fixed));
}



