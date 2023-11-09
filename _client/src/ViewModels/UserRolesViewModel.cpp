#include "UserRolesViewModel.hpp"

#include <QUrl>
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQuickItem>

#include "SpdlogWrapper.hpp"

UserRolesViewModel::UserRolesViewModel(QObject *parent)
    : QObject(parent)
{
}

UserRolesViewModel::~UserRolesViewModel()
{
}