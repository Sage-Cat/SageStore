#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"

#include "Localization/TsTranslator.hpp"
#include "Settings/AppSettings.hpp"
#include "common/Network/JsonSerializer.hpp"

#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainWindow.hpp"
#include "Ui/UiScale.hpp"
#include "Ui/Views/BaseView.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
int envOrDefaultNonNegativeInt(const char *name, int fallback)
{
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return fallback;
    }

    try {
        const int parsedValue = std::stoi(value);
        if (parsedValue < 0) {
            throw std::out_of_range("negative value");
        }
        return parsedValue;
    } catch (...) {
        SPDLOG_WARN("Invalid non-negative integer in {}='{}', using default {}", name, value,
                    fallback);
        return fallback;
    }
}

bool envOrDefaultBool(const char *name, bool fallback)
{
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return fallback;
    }

    std::string normalized(value);
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    if (normalized == "1" || normalized == "true" || normalized == "yes" || normalized == "on" ||
        normalized == "enable" || normalized == "enabled") {
        return true;
    }
    if (normalized == "0" || normalized == "false" || normalized == "no" || normalized == "off" ||
        normalized == "disable" || normalized == "disabled") {
        return false;
    }

    SPDLOG_WARN("Invalid boolean in {}='{}', using default {}", name, value, fallback);
    return fallback;
}
} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AppSettings::ensureApplicationMetadata();
    UiScale::configureApplication(app);

    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>(
        {.loggerName = "SageStoreClient", .logFileStem = "sagestore-client"});
    SPDLOG_INFO("SageStoreClient starting");

    const auto clientSettings = AppSettings::load();

    TsTranslator appTranslator;
    QTranslator qtTranslator;
    if (clientSettings.language == "ua") {
        if (appTranslator.loadFromResource(":/translations/SageStore_uk.ts")) {
            app.installTranslator(&appTranslator);
        } else {
            SPDLOG_WARN("Unable to load application translation for Ukrainian");
        }

        if (qtTranslator.load(QLocale("uk_UA"), "qtbase", "_",
                              QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
            app.installTranslator(&qtTranslator);
        }
    }

    // Networking
    const NetworkService::ServerConfig serverConfig = AppSettings::effectiveServerConfig();
    NetworkService networkService(serverConfig, std::make_unique<JsonSerializer>());
    ApiManager apiManager(networkService);
    networkService.init();

    const bool autoExitForced = envOrDefaultBool("SAGESTORE_FORCE_CLIENT_AUTO_EXIT_MS", false);
    const auto platformName = app.platformName().toStdString();
    const bool offscreen = (platformName == "offscreen");

    const int autoExitMs = envOrDefaultNonNegativeInt("SAGESTORE_CLIENT_AUTO_EXIT_MS", 0);
    if (autoExitMs > 0 && (autoExitForced || offscreen)) {
        SPDLOG_INFO("Client auto-exit enabled after {} ms", autoExitMs);
        QTimer::singleShot(autoExitMs, &app, &QCoreApplication::quit);
    } else if (autoExitMs > 0) {
        SPDLOG_INFO("Client auto-exit ignored for interactive platform '{}' (not offscreen)",
                    platformName);
    }

    // Dialog windows
    DialogManager dialogManager(apiManager);
    dialogManager.init();
    QObject::connect(&dialogManager, &DialogManager::loginCancelled, &app,
                     &QCoreApplication::quit);

    MainWindow mainWindow(app, apiManager, dialogManager);
    mainWindow.startUiProcess();

    const int exitCode = app.exec();
    SPDLOG_INFO("SageStoreClient finished with code={}", exitCode);
    SpdlogConfig::shutdown();
    return exitCode;
}
