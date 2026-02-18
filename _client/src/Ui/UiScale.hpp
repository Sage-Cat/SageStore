#pragma once

#include <QApplication>
#include <QGuiApplication>
#include <QRect>
#include <QScreen>
#include <QVariant>

#include <algorithm>
#include <cmath>

namespace UiScale {

inline qreal detectScaleFactor(const QScreen *screen)
{
    if (!screen) {
        return 1.0;
    }

    const QRect availableGeometry = screen->availableGeometry();
    if (!availableGeometry.isValid()) {
        return 1.0;
    }

    constexpr qreal BASE_WIDTH  = 1920.0;
    constexpr qreal BASE_HEIGHT = 1080.0;
    constexpr qreal MIN_SCALE   = 1.0;
    constexpr qreal MAX_SCALE   = 1.6;

    const qreal resolutionScale =
        std::min(static_cast<qreal>(availableGeometry.width()) / BASE_WIDTH,
                 static_cast<qreal>(availableGeometry.height()) / BASE_HEIGHT);

    // Soften the growth to avoid oversized controls on very large displays.
    const qreal softenedScale = 1.0 + std::max<qreal>(0.0, resolutionScale - 1.0) * 0.5;
    return std::clamp(softenedScale, MIN_SCALE, MAX_SCALE);
}

inline qreal appScaleFactor()
{
    constexpr char SCALE_PROPERTY[] = "uiScaleFactor";
    if (qApp) {
        const QVariant property = qApp->property(SCALE_PROPERTY);
        if (property.isValid()) {
            const qreal scaleFactor = property.toReal();
            if (scaleFactor > 0.0) {
                return scaleFactor;
            }
        }
    }
    return detectScaleFactor(QGuiApplication::primaryScreen());
}

inline int scalePx(int value)
{
    return std::max(1, static_cast<int>(std::lround(static_cast<qreal>(value) * appScaleFactor())));
}

inline QSize scaledWindowSize(const QSize &baseSize, const QRect &availableGeometry)
{
    QSize targetSize(scalePx(baseSize.width()), scalePx(baseSize.height()));
    if (!availableGeometry.isValid()) {
        return targetSize;
    }
    targetSize.setWidth(std::min(targetSize.width(), availableGeometry.width()));
    targetSize.setHeight(std::min(targetSize.height(), availableGeometry.height()));
    return targetSize;
}

inline void configureApplication(QApplication &app)
{
    constexpr char SCALE_PROPERTY[] = "uiScaleFactor";
    const qreal scaleFactor         = detectScaleFactor(app.primaryScreen());
    app.setProperty(SCALE_PROPERTY, scaleFactor);

    QFont appFont = app.font();
    qreal pointSize = appFont.pointSizeF();
    if (pointSize <= 0.0) {
        pointSize = 10.0;
    }
    appFont.setPointSizeF(pointSize * scaleFactor);
    app.setFont(appFont);
}

} // namespace UiScale
