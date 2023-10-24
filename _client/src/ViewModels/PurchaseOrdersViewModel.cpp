#include "PurchaseOrdersViewModel.hpp"

#include <QUrl>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include "SpdlogWrapper.hpp"

PurchaseOrdersViewModel::PurchaseOrdersViewModel(QObject *parent)
    : QObject(parent)
{
}

PurchaseOrdersViewModel::~PurchaseOrdersViewModel()
{
}