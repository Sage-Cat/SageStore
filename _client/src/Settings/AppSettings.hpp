#pragma once

#include <QString>

#include "Network/NetworkService.hpp"

struct ClientSettings {
    QString language{"en"};
    QString scheme{"http"};
    QString address{"127.0.0.1"};
    int port{8001};
};

class AppSettings {
public:
    static void ensureApplicationMetadata();
    static ClientSettings load();
    static void save(const ClientSettings &settings);
    static NetworkService::ServerConfig effectiveServerConfig();
};
