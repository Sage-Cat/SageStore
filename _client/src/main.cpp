#include "SageStoreClient.hpp"

#include <QApplication>
#include <QThread>

#include "ApiManager.hpp"
#include "UiManager.hpp"

#include "SpdlogConfig.hpp"

const QString SERVER_API_URL{"http://127.0.0.1:8001/api"};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SpdlogConfig::init();
    SPDLOG_INFO("SageStoreClient starting");

    // Networking
    ApiManager apiManager(SERVER_API_URL);
    QThread apiManagerThread;
    apiManager.moveToThread(&apiManagerThread);
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
