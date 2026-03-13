#include <QApplication>
#include <QTimer>

#include <cstdlib>
#include <stdexcept>
#include <string>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"

#include "common/Network/JsonSerializer.hpp"

#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainWindow.hpp"
#include "Ui/UiScale.hpp"
#include "Ui/Views/BaseView.hpp"

#include "common/SpdlogConfig.hpp"

namespace {
std::string envOrDefault(const char *name, const char *fallback)
{
    const char *value = std::getenv(name);
    return value != nullptr && *value != '\0' ? value : fallback;
}

int envOrDefaultPort(const char *name, int fallback)
{
    const char *value = std::getenv(name);
    if (value == nullptr || *value == '\0') {
        return fallback;
    }

    try {
        return std::stoi(value);
    } catch (...) {
        SPDLOG_WARN("Invalid port in {}='{}', using default {}", name, value, fallback);
        return fallback;
    }
}

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
    UiScale::configureApplication(app);

    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
    SPDLOG_INFO("SageStoreClient starting");

    // Networking
    const NetworkService::ServerConfig serverConfig{
        .scheme = envOrDefault("SAGESTORE_SERVER_SCHEME", "http"),
        .address = envOrDefault("SAGESTORE_SERVER_ADDRESS", "127.0.0.1"),
        .port = envOrDefaultPort("SAGESTORE_SERVER_PORT", 8001)};
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

    // MainWindow
    MainWindow mainWindow(app, apiManager, dialogManager);
    mainWindow.startUiProcess();

    // Handle closing
    QObject::connect(&app, &QCoreApplication::aboutToQuit, []() {
        SPDLOG_INFO("SageStoreClient finished with code=0");
    });

    return app.exec();
}
