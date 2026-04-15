#include "Ui/Views/ModuleStatusView.hpp"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {
QString toBulletListHtml(const QStringList &items)
{
    QString html = "<ul style='margin: 0; padding-left: 18px;'>";
    for (const auto &item : items) {
        html += "<li>" + item.toHtmlEscaped() + "</li>";
    }
    html += "</ul>";
    return html;
}

QWidget *createCard(const ModuleStatusView::Section &section, QWidget *parent)
{
    auto *card   = new QFrame(parent);
    auto *layout = new QVBoxLayout(card);

    auto *titleLabel = new QLabel(section.title, card);
    titleLabel->setObjectName("moduleStatusCardTitle");

    auto *bodyLabel = new QLabel(toBulletListHtml(section.items), card);
    bodyLabel->setObjectName("moduleStatusCardBody");
    bodyLabel->setTextFormat(Qt::RichText);
    bodyLabel->setWordWrap(true);
    bodyLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    layout->addWidget(titleLabel);
    layout->addWidget(bodyLabel);
    layout->setSpacing(8);
    layout->setContentsMargins(16, 16, 16, 16);

    card->setObjectName("moduleStatusCard");
    card->setProperty("card", true);
    return card;
}
} // namespace

ModuleStatusView::ModuleStatusView(const QString &title, const QString &statusBadge,
                                   const QString &summary, const Section &currentState,
                                   const Section &nextSteps, QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *headerRow  = new QHBoxLayout;

    auto *titleLabel = new QLabel(title, this);
    titleLabel->setObjectName("moduleStatusTitle");

    auto *badgeLabel = new QLabel(statusBadge, this);
    badgeLabel->setObjectName("moduleStatusBadge");

    headerRow->addWidget(titleLabel);
    headerRow->addStretch();
    headerRow->addWidget(badgeLabel);

    auto *summaryLabel = new QLabel(summary, this);
    summaryLabel->setObjectName("moduleStatusSummary");
    summaryLabel->setProperty("muted", true);
    summaryLabel->setWordWrap(true);

    mainLayout->addLayout(headerRow);
    mainLayout->addWidget(summaryLabel);
    mainLayout->addWidget(createCard(currentState, this));
    mainLayout->addWidget(createCard(nextSteps, this));
    mainLayout->addStretch();
    mainLayout->setSpacing(14);
    mainLayout->setContentsMargins(18, 18, 18, 18);

    setObjectName("moduleStatusView");
    setStyleSheet(
        "#moduleStatusView { background-color: transparent; }"
        "#moduleStatusTitle {"
        " color: #0f172a;"
        " font-size: 24px;"
        " font-weight: 700;"
        "}"
        "#moduleStatusBadge {"
        " background-color: #dbeafe;"
        " color: #1d4ed8;"
        " border-radius: 999px;"
        " padding: 6px 12px;"
        " font-weight: 600;"
        "}"
        "#moduleStatusCardTitle {"
        " color: #0f172a;"
        " font-size: 15px;"
        " font-weight: 700;"
        "}"
        "#moduleStatusCardBody {"
        " color: #475569;"
        " font-size: 13px;"
        "}"
    );
}
