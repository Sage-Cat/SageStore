import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: baseView
    width: parent ? parent.width : 0 
    height: parent ? parent.height : 0 
    
    property alias additionalButtonSpaceAlias: additionalButtonSpace
    property alias additionalUnderTableSpaceAlias: additionalUnderTableSpace

    Rectangle {
        anchors.fill: parent
        
        TableView {
            id: dataTable
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height * 0.6
            clip: true
        }

        RowLayout {
            anchors.top: dataTable.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 10

            Button {
                text: "Add"
                onClicked: {
                    // Implement add logic here
                }
                color: Style.currentTheme.secondaryColor
            }
            Button {
                text: "Delete"
                onClicked: {
                    // Implement delete logic here
                }
                color: Style.currentTheme.secondaryColor
            }
            Button {
                text: "Edit"
                onClicked: {
                    // Implement edit logic here
                }
                color: Style.currentTheme.secondaryColor
            }
            
            Item {
                id: additionalButtonSpace

                Layout.fillWidth: true
                // This space can be used by derived modules to add their custom buttons.
            }
        }

        Rectangle {
            id: additionalUnderTableSpace
            
            anchors.top: dataTable.bottom
            anchors.bottom: statusBar.top
            anchors.left: parent.left
            anchors.right: parent.right
            // This space can be used for additional module-specific Views, like text boxes.
        }

        Rectangle {
            id: statusBar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 30

            Label {
                anchors.centerIn: parent
                text: "Status: Ready"
                color: Style.currentTheme.textColor
            }
        }
        color: Style.currentTheme.secondaryColor
    }
}
