import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: "Main Window"

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Open"
            }
            MenuItem {
                text: "Close"
            }
        }
        Menu {
            title: "Edit"
            // ... other menu items
        }
        // ... additional menus
    }

    // Additional window components go here
}
