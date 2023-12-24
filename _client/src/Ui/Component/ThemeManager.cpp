#include "ThemeManager.hpp"

void ThemeManager::switchTheme(Theme theme)
{
    if (theme != currentTheme)
    {
        currentTheme = theme;
        applyTheme(currentTheme);
    }
}

void ThemeManager::applyTheme(Theme theme)
{
    QPalette palette;

    switch (theme)
    {
    case DarkTheme:
        palette.setColor(QPalette::Window, QColor("#121212"));
        palette.setColor(QPalette::WindowText, QColor("#FFFFFF"));
        palette.setColor(QPalette::Base, QColor("#1F1F1F")); // Secondary color for dark theme
        palette.setColor(QPalette::Text, QColor("#FFFFFF")); // Text color for dark theme
        break;

    case LightTheme:
        palette.setColor(QPalette::Window, QColor("#FFFFFF"));
        palette.setColor(QPalette::WindowText, QColor("#000000"));
        palette.setColor(QPalette::Base, QColor("#F1F1F1")); // Secondary color for light theme
        palette.setColor(QPalette::Text, QColor("#000000")); // Text color for light theme
        break;
    }

    QProxyStyle::polish(palette);
}