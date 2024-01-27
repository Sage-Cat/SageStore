#include "AuthorizationDialogModel.hpp"

#include "SpdlogWrapper.hpp"

AuthorizationDialogModel::AuthorizationDialogModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("AuthorizationDialogModel::AuthorizationDialogModel");
}
