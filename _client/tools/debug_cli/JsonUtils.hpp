#pragma once

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include "common/DataTypes.hpp"

namespace DebugCli::JsonUtils {

inline QString jsonScalarToString(const QJsonValue &value)
{
    if (value.isString()) {
        return value.toString();
    }
    if (value.isBool()) {
        return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    }
    if (value.isDouble()) {
        return QString::number(value.toDouble(), 'f', -1);
    }
    if (value.isNull() || value.isUndefined()) {
        return {};
    }

    if (value.isObject()) {
        return QString::fromUtf8(QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact));
    }
    if (value.isArray()) {
        return QString::fromUtf8(QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact));
    }

    return {};
}

inline Data jsonValueToData(const QJsonValue &value)
{
    Data row;
    if (value.isArray()) {
        const QJsonArray array = value.toArray();
        for (const QJsonValue &entry : array) {
            row.push_back(jsonScalarToString(entry).toStdString());
        }
        return row;
    }

    row.push_back(jsonScalarToString(value).toStdString());
    return row;
}

inline Dataset datasetFromJsonObject(const QJsonObject &object)
{
    Dataset dataset;
    for (auto it = object.begin(); it != object.end(); ++it) {
        dataset[it.key().toStdString()] = jsonValueToData(it.value());
    }
    return dataset;
}

inline QJsonArray dataToJsonArray(const Data &data)
{
    QJsonArray array;
    for (const std::string &value : data) {
        array.append(QString::fromStdString(value));
    }
    return array;
}

inline QJsonObject datasetToJsonObject(const Dataset &dataset)
{
    QJsonObject object;
    for (const auto &[key, value] : dataset) {
        object.insert(QString::fromStdString(key), dataToJsonArray(value));
    }
    return object;
}

inline bool readJsonFile(const QString &path, QJsonDocument *document, QString *errorMessage)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Unable to open JSON file: %1").arg(path);
        }
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument parsed = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        if (errorMessage != nullptr) {
            *errorMessage =
                QStringLiteral("Invalid JSON in %1: %2").arg(path, parseError.errorString());
        }
        return false;
    }

    if (document != nullptr) {
        *document = parsed;
    }
    return true;
}

inline bool writeJsonFile(const QString &path, const QJsonDocument &document,
                          QString *errorMessage)
{
    const QFileInfo fileInfo(path);
    if (!QDir().mkpath(fileInfo.absolutePath())) {
        if (errorMessage != nullptr) {
            *errorMessage =
                QStringLiteral("Unable to create output directory: %1").arg(fileInfo.absolutePath());
        }
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Unable to write JSON file: %1").arg(path);
        }
        return false;
    }

    file.write(document.toJson(QJsonDocument::Indented));
    return true;
}

} // namespace DebugCli::JsonUtils
