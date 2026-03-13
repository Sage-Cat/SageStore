#include "Settings/AppSettings.hpp"

#include <QCoreApplication>
#include <QSettings>

#include <cstdlib>

namespace {
QString envOrSetting(const char *envName, const QString &settingValue)
{
    const char *value = std::getenv(envName);
    if (value != nullptr && *value != '\0') {
        return QString::fromUtf8(value);
    }

    return settingValue;
}

int envOrSettingPort(const char *envName, int settingValue)
{
    const char *value = std::getenv(envName);
    if (value == nullptr || *value == '\0') {
        return settingValue;
    }

    bool ok = false;
    const int parsedValue = QString::fromUtf8(value).toInt(&ok);
    if (!ok || parsedValue <= 0) {
        return settingValue;
    }

    return parsedValue;
}
} // namespace

void AppSettings::ensureApplicationMetadata()
{
    if (QCoreApplication::organizationName().isEmpty()) {
        QCoreApplication::setOrganizationName("SageStore");
    }

    if (QCoreApplication::organizationDomain().isEmpty()) {
        QCoreApplication::setOrganizationDomain("sagestore.local");
    }

    if (QCoreApplication::applicationName().isEmpty()) {
        QCoreApplication::setApplicationName("SageStore");
    }
}

ClientSettings AppSettings::load()
{
    ensureApplicationMetadata();

    QSettings settings;
    ClientSettings value;
    value.language = settings.value("ui/language", value.language).toString();
    value.scheme   = settings.value("server/scheme", value.scheme).toString();
    value.address  = settings.value("server/address", value.address).toString();
    value.port     = settings.value("server/port", value.port).toInt();

    if (value.language != "en" && value.language != "ua") {
        value.language = "en";
    }

    if (value.port <= 0) {
        value.port = 8001;
    }

    return value;
}

void AppSettings::save(const ClientSettings &settingsValue)
{
    ensureApplicationMetadata();

    QSettings settings;
    settings.setValue("ui/language", settingsValue.language);
    settings.setValue("server/scheme", settingsValue.scheme.trimmed());
    settings.setValue("server/address", settingsValue.address.trimmed());
    settings.setValue("server/port", settingsValue.port);
    settings.sync();
}

NetworkService::ServerConfig AppSettings::effectiveServerConfig()
{
    const auto settings = load();

    return NetworkService::ServerConfig{
        .scheme = envOrSetting("SAGESTORE_SERVER_SCHEME", settings.scheme).toStdString(),
        .address = envOrSetting("SAGESTORE_SERVER_ADDRESS", settings.address).toStdString(),
        .port = envOrSettingPort("SAGESTORE_SERVER_PORT", settings.port)};
}
