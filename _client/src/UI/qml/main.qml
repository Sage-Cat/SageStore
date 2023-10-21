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
        }

        StackView {
            id: stackView
            initialItem: purchaseOrdersComponent  // Set your purchaseOrdersComponent as the initial item
            Layout.fillWidth: true  // Make sure this is set
            Layout.fillHeight: true

            Component.onCompleted: {
                stackView.push(purchaseOrdersView)
                stackView.push(supplierManagementView)
            }
        }

        // PurchaseOrdersView
        Component {
            id: purchaseOrdersComponent
            PurchaseOrdersView {  // This would be the root item in your PurchaseOrdersView.qml
            }
        }
        
        Rectangle {
            id: backgroundRect
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Style.currentTheme.secondaryColor
        }
    }
    
    Connections {
        target: uiManager
        function onThemeChanged(theme) {
            updateTheme(uiManager.theme);
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
