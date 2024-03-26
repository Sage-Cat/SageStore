#include "SageStoreClient.hpp"

#include <QApplication>
#include <QThread>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Network/JsonSerializer.hpp"
#include "Ui/UiManager.hpp"
#include "Ui/Views/BaseView.hpp"
#include "SpdlogConfig.hpp"

inline constexpr char ENDPOINT_SCHEME[] = "http";
inline constexpr char ENDPOINT_ADDR[]   = "127.0.0.1";
inline constexpr int  ENDPOINT_PORT     = 8001;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
    SPDLOG_INFO("SageStoreClient starting");

    const NetworkService::ServerConfig serverConfig{ .scheme = ENDPOINT_SCHEME, 
                                                     .address = ENDPOINT_ADDR, 
                                                     .port = ENDPOINT_PORT };
    NetworkService networkService(serverConfig, std::make_unique<JsonSerializer>());

    ApiManager apiManager(networkService);

    QThread apiManagerThread;
    apiManager.moveToThread(&apiManagerThread);
    networkService.moveToThread(&apiManagerThread);
    apiManagerThread.start();

    // Ui
    UiManager uiManager(app, apiManager);
    uiManager.init();

    // Orchestrator
    SageStoreClient client(apiManager, uiManager);
    client.start();

    QObject::connect(&app, &QCoreApplication::aboutToQuit,
                     [&apiManagerThread]()
                     {
                         apiManagerThread.quit();
                         apiManagerThread.wait();
                         SPDLOG_INFO("SageStoreClient finished with code=0");
                     });
    
    return app.exec();
}
