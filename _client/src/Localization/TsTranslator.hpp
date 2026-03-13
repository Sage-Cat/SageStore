#pragma once

#include <QHash>
#include <QTranslator>

class TsTranslator : public QTranslator {
public:
    explicit TsTranslator(QObject *parent = nullptr);

    bool loadFromResource(const QString &resourcePath);
    QString translate(const char *context, const char *sourceText,
                      const char *disambiguation = nullptr, int n = -1) const override;

private:
    static QString makeKey(const QString &context, const QString &source,
                           const QString &disambiguation);

    QHash<QString, QString> m_translations;
};
