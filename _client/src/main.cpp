#include "SageStoreClient.hpp"

#include <QApplication>
#include <QThread>

#include "Network/ApiManager.hpp"
#include "Network/NetworkService.hpp"
#include "Network/JsonSerializer.hpp"
#include "Ui/UiManager.hpp"

#include "SpdlogConfig.hpp"

const QString SERVER_API_URL{"http://127.0.0.1:8001/api"};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SpdlogConfig::init<SpdlogConfig::LogLevel::Trace>();
    SPDLOG_INFO("SageStoreClient starting");

    // Networking
    NetworkService networkService;
    networkService.setApiUrl(SERVER_API_URL);
    networkService.setSerializer(std::make_unique<JsonSerializer>());
    ApiManager apiManager(networkService);
    QThread apiManagerThread;
    apiManager.moveToThread(&apiManagerThread);
    networkService.moveToThread(&apiManagerThread);
    apiManagerThread.start();

    // Ui
    UiManager uiManager(app, apiManager);

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
