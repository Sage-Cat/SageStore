#include "PurchaseOrdersViewModel.hpp"

#include "SpdlogWrapper.hpp"

PurchaseOrdersViewModel::PurchaseOrdersViewModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("PurchaseOrdersViewModel::PurchaseOrdersViewModel");
}

PurchaseOrdersViewModel::~PurchaseOrdersViewModel()
{
    SPDLOG_TRACE("PurchaseOrdersViewModel::~PurchaseOrdersViewModel");
}
