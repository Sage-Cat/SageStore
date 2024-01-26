#include "SageStoreClient.hpp"
#include "SpdlogWrapper.hpp"
#include "Views/MainWindowView.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SpdlogWrapper::init();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app);

    return app.exec();
}
