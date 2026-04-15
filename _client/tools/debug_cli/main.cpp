#include <QApplication>
#include <QJsonDocument>
#include <QTextStream>

#include "DebugCliRunner.hpp"
#include "common/SpdlogConfig.hpp"

int main(int argc, char *argv[])
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("offscreen"));
    }

    QApplication application(argc, argv);
    application.setApplicationName(QStringLiteral("SageStoreDebugCli"));
    application.setOrganizationName(QStringLiteral("SageStore"));
    application.setOrganizationDomain(QStringLiteral("sagestore.local"));

    SpdlogConfig::init<SpdlogConfig::LogLevel::Info>(
        {.loggerName = "SageStoreDebugCli",
         .logFileStem = "sagestore-debug-cli",
         .enableConsole = false});

    DebugCli::DebugCliRunner runner(application);
    const auto result = runner.run(application.arguments());

    QTextStream(stdout) << QJsonDocument(result.document).toJson(QJsonDocument::Indented);
    SpdlogConfig::shutdown();
    return result.exitCode;
}
