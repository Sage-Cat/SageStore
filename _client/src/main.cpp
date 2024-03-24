#include "SageStoreClient.hpp"

#include <QApplication>
#include <QThread>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Network/JsonSerializer.hpp"
#include "Ui/UiManager.hpp"
#include "Ui/Views/BaseView.hpp"
#include "SpdlogConfig.hpp"

#define SERVER_SCHEME   "http"
#define SERVER_ADDR     "127.0.0.1"
#define SERVER_PORT     8001

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
    SPDLOG_INFO("SageStoreClient starting");

    const NetworkService::server_url_t serverUrl = { SERVER_SCHEME, 
                                                     SERVER_ADDR, 
                                                     SERVER_PORT };
    NetworkService networkService(serverUrl, std::make_unique<JsonSerializer>());

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
