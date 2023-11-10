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
        // Row {
        //     width: userRolesView.tableMod.contentWidth
        //     height: 40
        //     spacing: 2
        //     Repeater {
        //         model: table.model.columnWidth(index); height: parent.height
        //         //color: "blue";

        //         Text {
        //             anchors.verticalCenter: parent.verticalCenter
        //             x: 4;
        //             width: parent.width - 4;
        //             text: table.model.headerData(index, Qt.Horizontal)
        //         }
        //     }

        // }
        // model: TableModel {
        //     TableModelColumn { display: "name" }
        //     TableModelColumn { display: "color"}

        //     rows: [
        //         {
        //             "name": "cat",
        //             "color": "black"
        //         },
        //         // {
        //         //     "name": "dog",
        //         //     "color": "brown"
        //         // },
        //         {
        //             "name": "bird",
        //             "color": "white"
        //         }
        //     ]
        // }
        // column: TableColumn {
        //     role: "name"
        //     title: "Name"
        // }
        
        // TableViewColumn {
        //     role: "color"
        //     title: "Color"
        // }

        // delegate: Rectangle {
        //     implicitWidth: 100
        //     implicitHeight: 50
        //     Text {
        //         text: display
        //     }
            
        // }
    }
}