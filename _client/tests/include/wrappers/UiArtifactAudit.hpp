#pragma once

#include <QImageReader>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QSet>
#include <QString>
#include <QStringList>

#include <algorithm>

namespace UiArtifactAudit {
inline QString auditScreenshot(const QString &path, int minWidth = 320, int minHeight = 200,
                               int minSampledColors = 6)
{
    QImageReader reader(path);
    const QImage image = reader.read();
    if (image.isNull()) {
        return QStringLiteral("Unable to decode screenshot '%1': %2")
            .arg(path, reader.errorString());
    }

    if (image.width() < minWidth || image.height() < minHeight) {
        return QStringLiteral("Screenshot '%1' is too small: %2x%3")
            .arg(path, QString::number(image.width()), QString::number(image.height()));
    }

    const int stepX = std::max(1, image.width() / 24);
    const int stepY = std::max(1, image.height() / 24);
    QSet<QRgb> sampledColors;
    for (int y = 0; y < image.height(); y += stepY) {
        for (int x = 0; x < image.width(); x += stepX) {
            sampledColors.insert(image.pixel(x, y));
        }
    }

    if (sampledColors.size() < minSampledColors) {
        return QStringLiteral("Screenshot '%1' looks blank or monochrome (%2 sampled colors)")
            .arg(path, QString::number(sampledColors.size()));
    }

    return {};
}

inline void collectObjectNames(const QJsonValue &value, QSet<QString> *objectNames)
{
    if (objectNames == nullptr) {
        return;
    }

    if (value.isObject()) {
        const QJsonObject object = value.toObject();
        const QString objectName = object.value(QStringLiteral("objectName")).toString();
        if (!objectName.isEmpty()) {
            objectNames->insert(objectName);
        }

        for (auto it = object.begin(); it != object.end(); ++it) {
            collectObjectNames(it.value(), objectNames);
        }
        return;
    }

    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &entry : array) {
            collectObjectNames(entry, objectNames);
        }
    }
}

inline QString auditStateDocument(const QJsonDocument &document,
                                  const QStringList &expectedObjectNames = {})
{
    if (!document.isObject()) {
        return QStringLiteral("State artifact is not a JSON object");
    }

    const QJsonObject root = document.object();
    if (!root.contains(QStringLiteral("selectedRoot")) ||
        !root.value(QStringLiteral("selectedRoot")).isObject()) {
        return QStringLiteral("State artifact does not contain a selectedRoot object");
    }

    QSet<QString> objectNames;
    collectObjectNames(root, &objectNames);
    QStringList missing;
    for (const QString &expectedObjectName : expectedObjectNames) {
        if (!expectedObjectName.isEmpty() && !objectNames.contains(expectedObjectName)) {
            missing.append(expectedObjectName);
        }
    }

    if (!missing.isEmpty()) {
        return QStringLiteral("State artifact is missing expected object names: %1")
            .arg(missing.join(QStringLiteral(", ")));
    }

    return {};
}
} // namespace UiArtifactAudit
