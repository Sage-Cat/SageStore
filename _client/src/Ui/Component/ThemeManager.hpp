#pragma once

#include <QProxyStyle>
#include <QColor>

class ThemeManager : public QProxyStyle
{
public:
    enum Theme
    {
        DarkTheme,
        LightTheme
    };

    ThemeManager() : currentTheme(DarkTheme)
    {
        applyTheme(currentTheme);
    }

    void switchTheme(Theme theme);

private:
    void applyTheme(Theme theme);

    Theme currentTheme;
};