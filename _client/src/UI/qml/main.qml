import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import com.sage 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200
    height: 800
    title: "SageStore"
    flags: Qt.Window
    visibility: "Maximized"

    property color darkPrimaryColor: "#121212"
    property color darkSecondaryColor: "#1F1F1F"
    property color darkTextColor: "#FFFFFF"

    property color lightPrimaryColor: "#FFFFFF"
    property color lightSecondaryColor: "#F1F1F1"
    property color lightTextColor: "#000000"

    ColumnLayout {
        anchors.fill: parent
        
        MainMenu {  
            Layout.fillWidth: true
        }

        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            initialItem: Item {}
        }
        
        Rectangle {
            id: backgroundRect
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: mainWindow.color
        }
    }
    
    Connections {
        target: uiManager
        function onThemeChanged(theme) {
            updateTheme(uiManager.theme);
        }
    }

    function updateTheme(theme) {
        if (theme === UIManager.Theme.Dark) {
            mainWindow.color = darkPrimaryColor;
            backgroundRect.color = darkSecondaryColor;
            MainMenu.color = darkPrimaryColor;
            // mainMenuBar.text.color = darkTextColor; 
        } else {
            mainWindow.color = lightPrimaryColor;
            backgroundRect.color = lightSecondaryColor;
            MainMenu.color = lightPrimaryColor;
            // mainMenuBar.text.color = lightTextColor; 
        }
    }
}
