import QtQuick.Controls 2.15

import "qrc:/"
import "qrc:/Components/"

BaseView {
    id: purchaseOrdersView  // Change the ID to match the reference below

    // Adding additional button to `additionalButtonSpace`
    Button {
        parent: purchaseOrdersView.additionalButtonSpaceAlias
        anchors.verticalCenter: parent.verticalCenter
        text: "Upload Invoice"
        onClicked: {
            // Implement upload logic here
        }
    }

    Label {
        parent: purchaseOrdersView.additionalUnderTableSpaceAlias  
        anchors.right: parent.right 
        text: "Additional Purchase Info"
        color: Style.currentTheme.textColor
    }
}
