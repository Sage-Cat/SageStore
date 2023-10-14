#include "SageStoreClient.hpp"
#include "Logging.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Logging::init();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app);
    client.init();

    return app.exec();
}
