#include "Localization/TsTranslator.hpp"

#include <QFile>
#include <QXmlStreamReader>

TsTranslator::TsTranslator(QObject *parent) : QTranslator(parent) {}

bool TsTranslator::loadFromResource(const QString &resourcePath)
{
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    m_translations.clear();

    QXmlStreamReader xml(&file);
    QString currentContext;

    while (!xml.atEnd()) {
        xml.readNext();

        if (!xml.isStartElement()) {
            continue;
        }

        if (xml.name() == QStringView(u"context")) {
            currentContext.clear();
            QString sourceText;
            QString translationText;
            QString disambiguation;

            while (!(xml.isEndElement() && xml.name() == QStringView(u"context")) && !xml.atEnd()) {
                xml.readNext();

                if (!xml.isStartElement()) {
                    continue;
                }

                if (xml.name() == QStringView(u"name")) {
                    currentContext = xml.readElementText();
                    continue;
                }

                if (xml.name() != QStringView(u"message")) {
                    continue;
                }

                sourceText.clear();
                translationText.clear();
                disambiguation.clear();

                while (!(xml.isEndElement() && xml.name() == QStringView(u"message")) &&
                       !xml.atEnd()) {
                    xml.readNext();

                    if (!xml.isStartElement()) {
                        continue;
                    }

                    if (xml.name() == QStringView(u"source")) {
                        sourceText = xml.readElementText();
                    } else if (xml.name() == QStringView(u"translation")) {
                        translationText = xml.readElementText();
                    } else if (xml.name() == QStringView(u"comment")) {
                        disambiguation = xml.readElementText();
                    }
                }

                if (!currentContext.isEmpty() && !sourceText.isEmpty() &&
                    !translationText.isEmpty()) {
                    m_translations.insert(makeKey(currentContext, sourceText, disambiguation),
                                          translationText);
                }
            }
        }
    }

    return !xml.hasError();
}

QString TsTranslator::translate(const char *context, const char *sourceText,
                                const char *disambiguation, int n) const
{
    Q_UNUSED(n);

    const QString key = makeKey(QString::fromUtf8(context), QString::fromUtf8(sourceText),
                                disambiguation == nullptr ? QString()
                                                          : QString::fromUtf8(disambiguation));
    return m_translations.value(key);
}

QString TsTranslator::makeKey(const QString &context, const QString &source,
                              const QString &disambiguation)
{
    return context + QChar(0x1F) + source + QChar(0x1F) + disambiguation;
}
