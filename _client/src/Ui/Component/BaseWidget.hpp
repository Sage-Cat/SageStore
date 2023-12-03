#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QSpacerItem>

class BaseWidget : public QWidget
{
    Q_OBJECT

public:
    BaseWidget(QWidget *parent = nullptr);

private:
    // setup
    void setupUi();
    void setupConnections();

protected:
    // layputs
    QVBoxLayout *_mMainLayout = new QVBoxLayout(this);
    QHBoxLayout *_mButtonRow = new QHBoxLayout;
    QHBoxLayout *_mAdditionalInfo = new QHBoxLayout;
    // buttons
    QPushButton *_mAddButton = new QPushButton("Add", this);
    QPushButton *_mDeleteButton = new QPushButton("Delete", this);
    QPushButton *_mEditButton = new QPushButton("Edit", this);
    // data table
    QTableWidget *_mDataTable = new QTableWidget(this);
    // additional space
    QSpacerItem *_mAdditionalButtonSpace = new QSpacerItem(100, 50, QSizePolicy::Expanding, QSizePolicy::Fixed);
    QSpacerItem *_mAdditionalUnderTableSpace = new QSpacerItem(100, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    // label
    QLabel *_mStatus = new QLabel("Status: Ready", this);

signals:

public slots:
};

#endif // BASEWIDGET_H