#include "AuthorizationViewModel.hpp"

#include "SpdlogWrapper.hpp"

AuthorizationViewModel::AuthorizationViewModel(QObject *parent)
    : QObject(parent)
{
    SPDLOG_TRACE("AuthorizationViewModel::AuthorizationViewModel");
}
