#include "SageStoreClient.hpp"

#include "SpdlogConfig.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SpdlogConfig::init();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app);

    return app.exec();
}
