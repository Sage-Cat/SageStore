#include "SageStoreClient.hpp"
#include "statements.hpp"

void initSpdlog()
{
    auto console = spdlog::stdout_color_mt("console");
    console->set_level(spdlog::level::trace);
    console->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    initSpdlog();
    SPDLOG_INFO("SageStoreClient starting");

    SageStoreClient client(app);
    client.init();

    return app.exec();
}
