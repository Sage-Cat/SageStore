import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import "qrc:/"

Rectangle {
    anchors.fill: parent
    color: Style.currentTheme.secondaryColor
    
    property alias additionalButtonSpaceAlias: additionalButtonSpace
    property alias additionalUnderTableSpaceAlias: additionalUnderTableSpace
    property alias tableMod: dataTable
    
    // Use a vertical layout to structure the main items
    ColumnLayout {
        anchors.fill: parent

        // Button row
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            spacing: 10

            Button { text: "Add"; /*...*/ }
            Button { text: "Delete"; /*...*/ }
            Button { text: "Edit"; /*...*/ }
            
            Item {
                id: additionalButtonSpace
                Layout.fillWidth: true
                Layout.preferredHeight: 50
            }
        }

        // Your data table
        Rectangle {
            id: dataTable
            Layout.fillWidth: true
            Layout.fillHeight: true
            Row {
                //header
                width: userRolesView.tableMod.contentWidth
                height: 40
                spacing: 2
                Repeater {
                    model: table.model.columnWidth(index); height: parent.height
                    //color: "blue";

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        x: 4;
                        width: parent.width - 4;
                        text: table.model.headerData(index, Qt.Horizontal)
                    }
                }
            }
            TableView {
                //data
                model: TableModel {
                    // TableModelColumn { display: "name" }
                    // TableModelColumn { display: "color"}

                    rows: [
                        {
                            "name": "cat",
                            "color": "black"
                        },
                        // {
                        //     "name": "dog",
                        //     "color": "brown"
                        // },
                        {
                            "name": "bird",
                            "color": "white"
                        }
                    ]
                }
                clip: true
            }
        
        }

        // Additional space
        Rectangle {
            id: additionalUnderTableSpace
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: Style.currentTheme.secondaryColor
        }

        // Status bar
        Rectangle {
            id: statusBar
            Layout.fillWidth: true
            Layout.preferredHeight: 20
            color: Style.currentTheme.secondaryColor

            Label {
                anchors.centerIn: parent
                text: "Status: Ready"
                color: Style.currentTheme.textColor
            }
        }
    }
}

