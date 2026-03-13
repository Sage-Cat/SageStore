#pragma once

#include <QString>
#include <QStringList>
#include <QWidget>

class ModuleStatusView : public QWidget {
public:
    struct Section {
        QString title;
        QStringList items;
    };

    explicit ModuleStatusView(const QString &title, const QString &statusBadge,
                              const QString &summary, const Section &currentState,
                              const Section &nextSteps, QWidget *parent = nullptr);
};
