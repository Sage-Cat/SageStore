import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import com.sage 1.0

import "Components"
import "Views"

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200
    height: 800
    title: "SageStore"
    flags: Qt.Window
    visibility: "Maximized"

    ColumnLayout {
        anchors.fill: parent
        
        MainMenu {  
            Layout.fillWidth: true
            Layout.preferredHeight: 50  // Just an example; adjust based on your needs
        }

        StackView {
            id: stackView
            // initialItem: purchaseOrdersComponent 
            Layout.fillWidth: true
            Layout.fillHeight: true

            Component.onCompleted: {
                stackView.push(purchaseOrdersComponent) // push initial item
            }

            // PurchaseOrdersView
            Component {
                id: purchaseOrdersComponent
                PurchaseOrdersView {  
                }
            }
        }
    }

    Connections {
        target: uiManager
        function onThemeChanged(theme) {
            updateTheme(uiManager.theme);
        }
    }

    function updateTheme(theme) {
        if (theme === UiManager.Theme.Dark) {
            Style.currentTheme = Style.darkTheme;
        } else {
            Style.currentTheme = Style.lightTheme;
        }
    }
}
