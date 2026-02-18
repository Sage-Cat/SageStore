#include <QApplication>
#include <QThread>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"

#include "common/Network/JsonSerializer.hpp"

#include "Ui/Dialogs/DialogManager.hpp"
#include "Ui/MainWindow.hpp"
#include "Ui/UiScale.hpp"
#include "Ui/Views/BaseView.hpp"

#include "common/SpdlogConfig.hpp"

inline constexpr char ENDPOINT_SCHEME[] = "http";
inline constexpr char ENDPOINT_ADDR[]   = "127.0.0.1";
inline constexpr int ENDPOINT_PORT      = 8001;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    UiScale::configureApplication(app);

    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
    SPDLOG_INFO("SageStoreClient starting");

    // Networking
    const NetworkService::ServerConfig serverConfig{
        .scheme = ENDPOINT_SCHEME, .address = ENDPOINT_ADDR, .port = ENDPOINT_PORT};
    NetworkService networkService(serverConfig, std::make_unique<JsonSerializer>());
    ApiManager apiManager(networkService);
    QThread apiManagerThread;
    QObject::connect(&apiManagerThread, &QThread::started, &networkService, &NetworkService::init);
    apiManager.moveToThread(&apiManagerThread);
    networkService.moveToThread(&apiManagerThread);
    apiManagerThread.start();

    // Dialog windows
    DialogManager dialogManager(apiManager);
    dialogManager.init();

    // MainWindow
    MainWindow mainWindow(app, apiManager, dialogManager);
    mainWindow.startUiProcess();

    // Handle closing
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&apiManagerThread]() {
        apiManagerThread.quit();
        apiManagerThread.wait();
        SPDLOG_INFO("SageStoreClient finished with code=0");
    });

    return app.exec();
}
