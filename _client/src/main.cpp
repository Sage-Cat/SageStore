#include "SageStoreClient.hpp"
#include "SpdlogWrapper.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SpdlogWrapper::init();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app);
    client.init();

    return app.exec();
}
