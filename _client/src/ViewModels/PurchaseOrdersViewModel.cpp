#include "PurchaseOrdersViewModel.hpp"

#include <spdlog/spdlog.h>

PurchaseOrdersViewModel::PurchaseOrdersViewModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("PurchaseOrdersViewModel::PurchaseOrdersViewModel");
}

PurchaseOrdersViewModel::~PurchaseOrdersViewModel()
{
    SPDLOG_TRACE("PurchaseOrdersViewModel::~PurchaseOrdersViewModel");
}
