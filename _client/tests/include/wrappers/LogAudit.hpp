#pragma once

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QString>
#include <QStringList>
#include <QVector>

namespace LogAudit {
struct Finding {
    QString path;
    int lineNumber{0};
    QString line;
};

inline bool shouldIgnoreLine(const QString &line, const QStringList &ignoredSubstrings)
{
    for (const QString &ignored : ignoredSubstrings) {
        if (!ignored.isEmpty() && line.contains(ignored, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}

inline QVector<Finding> scanFile(const QString &path, const QStringList &ignoredSubstrings = {})
{
    static const QRegularExpression kErrorPattern(
        QStringLiteral(R"(\[(error|critical)\])"),
        QRegularExpression::CaseInsensitiveOption);

    QVector<Finding> findings;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return findings;
    }

    int lineNumber = 0;
    while (!file.atEnd()) {
        const QString line = QString::fromUtf8(file.readLine()).trimmed();
        ++lineNumber;
        if (line.isEmpty() || shouldIgnoreLine(line, ignoredSubstrings) ||
            !kErrorPattern.match(line).hasMatch()) {
            continue;
        }

        findings.append(Finding{.path = path, .lineNumber = lineNumber, .line = line});
    }

    return findings;
}

inline QVector<Finding> collectFindings(const QStringList &paths,
                                        const QStringList &ignoredSubstrings = {})
{
    QVector<Finding> findings;
    for (const QString &path : paths) {
        if (path.isEmpty()) {
            continue;
        }

        const QFileInfo info(path);
        if (!info.exists()) {
            continue;
        }

        if (info.isDir()) {
            QDirIterator iterator(path, QStringList{QStringLiteral("*.log")},
                                  QDir::Files | QDir::NoDotAndDotDot,
                                  QDirIterator::Subdirectories);
            while (iterator.hasNext()) {
                findings += scanFile(iterator.next(), ignoredSubstrings);
            }
            continue;
        }

        findings += scanFile(info.absoluteFilePath(), ignoredSubstrings);
    }

    return findings;
}

inline QString formatFindings(const QVector<Finding> &findings)
{
    QStringList lines;
    for (const Finding &finding : findings) {
        lines.append(QStringLiteral("%1:%2 %3")
                         .arg(QFileInfo(finding.path).fileName(), QString::number(finding.lineNumber),
                              finding.line));
    }

    return lines.join(QLatin1Char('\n'));
}
} // namespace LogAudit
