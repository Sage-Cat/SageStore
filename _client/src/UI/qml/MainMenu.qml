import QtQuick.Controls

MenuBar {
    Menu {
        title: "File"
        MenuItem { 
            text: "Settings"
        }
        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }
    Menu {
        title: "Purchasing Module"
        MenuItem {
            text: "Add Purchase"
        }
        MenuItem {
            text: "Edit Purchase"
        }
        MenuItem {
            text: "Delete Purchase"
        }
    }
    Menu {
        title: "Sales Module"
        MenuItem {
            text: "Add Sale"
        }
        MenuItem {
            text: "Edit Sale"
        }
        MenuItem {
            text: "Delete Sale"
        }
        MenuItem {
            text: "Audit Trail"
        }
    }
    Menu {
        title: "Inventory Module"
        MenuItem {
            text: "Add Item"
        }
        MenuItem {
            text: "Edit Item"
        }
        MenuItem {
            text: "Delete Item"
        }
    }
    Menu {
        title: "Analytics Module"
        MenuItem {
            text: "Gross Revenue"
        }
        MenuItem {
            text: "Employee Actions Log"
        }
    }
    Menu {
        title: "User Management"
        MenuItem {
            text: "Add User"
        }
        MenuItem {
            text: "Edit User"
        }
        MenuItem {
            text: "Delete User"
        }
    }
    Menu {
        title: "HR Management"
        MenuItem {
            text: "Add Employee"
        }
        MenuItem {
            text: "Edit Employee"
        }
        MenuItem {
            text: "Delete Employee"
        }
    }
    Menu {
        title: "Customer Management"
        MenuItem {
            text: "Add Customer"
        }
        MenuItem {
            text: "Edit Customer"
        }
        MenuItem {
            text: "Delete Customer"
        }
    }
    Menu {
        title: "Printing"
        MenuItem {
            text: "Print Forms"
        }
        MenuItem {
            text: "Print Labels"
        }
    }
    Menu {
        title: "Help"
        MenuItem {
            text: "About"
        }
        MenuItem {
            text: "Contact Us"
        }
    }
}