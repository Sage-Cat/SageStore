#include "SageStoreClient.hpp"
#include "logging.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    logging::init();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app);
    client.init();

    return app.exec();
}
