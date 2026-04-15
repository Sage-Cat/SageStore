#pragma once

#include <QApplication>
#include <QJsonObject>
#include <QStringList>

#include "ServerController.hpp"
#include "UiAutomation.hpp"

namespace DebugCli {

class DebugCliRunner {
public:
    struct RunResult {
        int exitCode{0};
        QJsonObject document;
    };

    explicit DebugCliRunner(QApplication &application);

    RunResult run(const QStringList &arguments);

private:
    RunResult usageError(const QString &message) const;
    RunResult runDescribe() const;
    RunResult runScenario(const QString &scenarioPath, const QString &resultPath,
                          const QString &artifactDir);
    QString resolveOutputPath(const QString &rawPath, const QDir &scenarioDir,
                              const QString &artifactDir) const;
    int exitCodeForCategory(const QString &category) const;

    QApplication &m_application;
    ServerController m_serverController;
    UiAutomation m_uiAutomation;
};

} // namespace DebugCli
