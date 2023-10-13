#include "SageStoreClient.hpp"
#include "statements.hpp"

void initSpdlog()
{
    auto console = spdlog::stdout_color_mt("console");
    console->set_level(spdlog::level::trace);
    console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
    console->flush_on(spdlog::level::trace);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    initSpdlog();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app); // Pass the QApplication instance to SageStoreClient
    client.init();

    // Lifecycle management
    QObject::connect(&client, &SageStoreClient::started, []()
                     { SPDLOG_INFO("SageStoreClient has started"); });
    QObject::connect(&client, &SageStoreClient::finished, [&app]()
                     {SPDLOG_INFO("SageStoreClient is finishing"); app.quit(); });
    QObject::connect(&app, &QCoreApplication::aboutToQuit, []()
                     { SPDLOG_INFO("SageStoreClient finished with code=0"); });

    return app.exec();
}
