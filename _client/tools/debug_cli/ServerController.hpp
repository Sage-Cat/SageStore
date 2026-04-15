#pragma once

#include <QJsonObject>
#include <QProcess>
#include <QString>
#include <QTemporaryDir>

#include <memory>

#include "Network/NetworkService.hpp"
#include "OperationResult.hpp"

namespace DebugCli {

class ServerController {
public:
    struct Config {
        QString mode{QStringLiteral("target")};
        QString scheme{QStringLiteral("http")};
        QString address{QStringLiteral("127.0.0.1")};
        int port{8001};
        QString binaryPath;
        QString sqlPath;
        QString dbPath;
        int startupTimeoutMs{10000};
    };

    ServerController();
    ~ServerController();

    static Config configFromJson(const QJsonObject &object);

    OperationResult prepare(const Config &config);
    OperationResult request(const QString &method, const QString &endpoint, const Dataset &dataset,
                            const QString &resourceId, int timeoutMs);
    OperationResult health(int timeoutMs);

    void stop();
    bool isPrepared() const;
    QJsonObject state() const;
    NetworkService::ServerConfig serverConfig() const;

private:
    static quint16 reservePort();
    QString logContents() const;
    Config normalizedConfig(const Config &config) const;

    Config m_config;
    NetworkService::ServerConfig m_effectiveConfig{"http", "127.0.0.1", 8001};
    QProcess m_process;
    std::unique_ptr<QTemporaryDir> m_tempDir;
    QString m_logPath;
    bool m_prepared{false};
};

} // namespace DebugCli
