#pragma once

#include <QHash>
#include <QStringList>

using Data = QStringList;
using Dataset = QHash<QString, Data>;

using ResponseHandler = std::function<void(const Dataset &)>;