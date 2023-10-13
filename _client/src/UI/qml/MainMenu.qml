import QtQuick.Controls 2.15

MenuBar {
    id: mainMenuBar

    // File Menu
    Menu {
        title: "File"
        MenuItem { text: "Settings" }
        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }

    // Purchasing Module Menu
    Menu {
        title: "Purchasing"
        MenuItem { text: "Purchase Orders" }
        MenuItem { text: "Supplier Management" }
        MenuItem { text: "Goods Receipts" }
    }

    // Sales Module Menu
    Menu {
        title: "Sales"
        MenuItem { text: "Sales Orders" }
        MenuItem { text: "Customer Management" }
        MenuItem { text: "Invoicing" }
    }

    // Inventory Module Menu
    Menu {
        title: "Inventory"
        MenuItem { text: "Product Management" }
        MenuItem { text: "Supplier's Pricelist Upload" }
        MenuItem { text: "Stock Tracking" }
    }

    // Analytics Module Menu
    Menu {
        title: "Analytics"
        MenuItem { text: "Sales Analytics" }
        MenuItem { text: "Inventory Analytics" }
    }

    // Users Module Menu
    Menu {
        title: "Users"
        MenuItem { text: "User Roles" }
        MenuItem { text: "User Logs" }
    }

    // Management Module Menu
    Menu {
        title: "Management"
        MenuItem { text: "Employees" }
        MenuItem { text: "Customers" }
        MenuItem { text: "Suppliers" }
    }
}
