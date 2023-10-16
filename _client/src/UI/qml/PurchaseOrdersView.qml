import QtQuick.Controls 2.15
import "Components"

BaseView {
    id: purchaseOrdersView  // Change the ID to match the reference below
    width: parent ? parent.width : 0 
    height: parent ? parent.height : 0 

    // Adding additional button to `additionalButtonSpace`
    Button {
        parent: purchaseOrdersView.additionalButtonSpaceAlias
        text: "Upload Invoice"
        onClicked: {
            // Implement upload logic here
        }
    }

    Label {
        parent: purchaseOrdersView.additionalUnderTableSpaceAlias  // Set the parent first
        anchors.centerIn: parent  // Now, anchor it to its parent
        text: "Additional Purchase Info"
    }
}
