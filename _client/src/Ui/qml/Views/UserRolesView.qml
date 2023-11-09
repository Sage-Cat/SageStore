import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels

import "qrc:/"
import "qrc:/Components/"
//import "qrc:/Components/TableModel"

//import Qt.labs.qmlmodels

BaseView {
    id: userRolesView  // Change the ID to match the reference below

    TableView {
        parent: userRolesView.tableMod
        anchors.fill: parent
        columnSpacing: 1
        rowSpacing: 1
        clip: true

        model: TableModel {
            TableModelColumn { display: "name"}
            TableModelColumn { display: "color"}

            rows: [
                {
                    "name": "cat",
                    "color": "black"
                },
                {
                    "name": "dog",
                    "color": "brown"
                },
                {
                    "name": "bird",
                    "color": "white"
                }
            ]
        }
    

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 50
            Text {
                text: display
            }
            MouseArea {
            anchors.fill: parent
            onClicked: {
                ignore;
            }
            property bool isPressed: false

            onPressed: {
                isPressed = true;
            }

            onReleased: {
                if (isPressed) {
                    isPressed = false;
                    ignore;
                }
            }
        }
        }
    }
}