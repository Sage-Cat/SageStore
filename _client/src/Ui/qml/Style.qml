// Style.qml
pragma Singleton
import QtQuick 2.0

QtObject {
    property QtObject darkTheme: QtObject {
        property color primaryColor: "#121212"
        property color secondaryColor: "#1F1F1F"
        property color textColor: "#FFFFFF"
    }

    property QtObject lightTheme: QtObject {
        property color primaryColor: "#FFFFFF"
        property color secondaryColor: "#F1F1F1"
        property color textColor: "#000000"
    }

    property QtObject currentTheme: darkTheme  // default theme
}