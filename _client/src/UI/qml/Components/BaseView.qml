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
        color: "transparent"
        
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
            }
            Button {
                text: "Delete"
                onClicked: {
                    // Implement delete logic here
                }
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
            color: "transparent"
            // This space can be used for additional module-specific Views, like text boxes.
        }

        Rectangle {
            id: statusBar
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 30
            color: "#EEE"

            Label {
                anchors.centerIn: parent
                text: "Status: Ready"
            }
        }
    }
}
