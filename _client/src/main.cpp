#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

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
#include "Ui/StartupController.hpp"
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
} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AppSettings::ensureApplicationMetadata();
    UiScale::configureApplication(app);

    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
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

    const int autoExitMs = envOrDefaultNonNegativeInt("SAGESTORE_CLIENT_AUTO_EXIT_MS", 0);
    if (autoExitMs > 0) {
        SPDLOG_INFO("Client auto-exit enabled after {} ms", autoExitMs);
        QTimer::singleShot(autoExitMs, &app, &QCoreApplication::quit);
    }

    // Dialog windows
    DialogManager dialogManager(apiManager);
    dialogManager.init();
    QObject::connect(&dialogManager, &DialogManager::loginCancelled, &app,
                     &QCoreApplication::quit);

    std::unique_ptr<MainWindow> mainWindow;
    StartupController startupController(dialogManager, [&]() {
        mainWindow = std::make_unique<MainWindow>(app, apiManager, dialogManager);
        return mainWindow.get();
    });
    startupController.start();

    // Handle closing
    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        SPDLOG_INFO("SageStoreClient finished with code=0");
    });

    return app.exec();
}
