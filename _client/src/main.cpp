#include "SageStoreClient.hpp"
#include "SpdlogWrapper.hpp"
#include "Ui/MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    SpdlogWrapper::init();
    SPDLOG_INFO("SageStoreClient starting");

    // SageStoreClient client(app);
    // client.init();

    MainWindow *test = new MainWindow();
    test->show();

    return app.exec();
}
