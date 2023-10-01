-- Enable Foreign Key Support
PRAGMA foreign_keys = ON;

-- Create Tables with Constraints
-- Roles Table
CREATE TABLE IF NOT EXISTS Roles (
    RoleID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL UNIQUE
);

-- Users Table with Foreign Key Constraints and Unique username
CREATE TABLE IF NOT EXISTS Users (
    UserID INTEGER PRIMARY KEY AUTOINCREMENT,
    Username TEXT UNIQUE NOT NULL,
    Password TEXT NOT NULL,  -- Note: Should be stored as a hashed value
    RoleID INTEGER NOT NULL,
    FOREIGN KEY (RoleID) REFERENCES Roles (RoleID) ON DELETE RESTRICT ON UPDATE CASCADE
);

CREATE UNIQUE INDEX idx_users_username ON Users(Username);

-- User Logs
CREATE TABLE IF NOT EXISTS UserLogs (
    LogID INTEGER PRIMARY KEY AUTOINCREMENT,
    UserID INTEGER NOT NULL,
    Action TEXT NOT NULL,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (UserID) REFERENCES Users (UserID) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Contacts
CREATE TABLE IF NOT EXISTS Contacts (
    ContactID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL,
    Type TEXT NOT NULL,
    Email TEXT UNIQUE,
    Phone TEXT UNIQUE
);

-- Contact Cars
CREATE TABLE IF NOT EXISTS ContactCars (
    CarID INTEGER PRIMARY KEY AUTOINCREMENT,
    ContactID INTEGER NOT NULL,
    LicensePlate TEXT UNIQUE NOT NULL,
    Model TEXT NOT NULL,
    FOREIGN KEY (ContactID) REFERENCES Contacts (ContactID) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Suppliers
CREATE TABLE IF NOT EXISTS Suppliers (
    SupplierID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL,
    ContactInfo TEXT
);

-- Products
CREATE TABLE IF NOT EXISTS Products (
    ProductID INTEGER PRIMARY KEY AUTOINCREMENT,
    Name TEXT NOT NULL,
    Description TEXT,
    Category TEXT,
    SupplierID INTEGER,
    FOREIGN KEY (SupplierID) REFERENCES Suppliers (SupplierID) ON DELETE SET NULL ON UPDATE CASCADE
);

-- Inventory
CREATE TABLE IF NOT EXISTS Inventory (
    InventoryID INTEGER PRIMARY KEY AUTOINCREMENT,
    ProductID INTEGER NOT NULL,
    QuantityAvailable INTEGER NOT NULL CHECK (QuantityAvailable >= 0),
    FOREIGN KEY (ProductID) REFERENCES Products (ProductID) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Orders
CREATE TABLE IF NOT EXISTS Orders (
    OrderID INTEGER PRIMARY KEY AUTOINCREMENT,
    Date DATETIME DEFAULT CURRENT_TIMESTAMP,
    UserID INTEGER NOT NULL,
    CustomerID INTEGER NOT NULL,
    Status TEXT NOT NULL,
    Type TEXT NOT NULL,
    FOREIGN KEY (UserID) REFERENCES Users (UserID) ON DELETE RESTRICT ON UPDATE CASCADE,
    FOREIGN KEY (CustomerID) REFERENCES Contacts (ContactID) ON DELETE RESTRICT ON UPDATE CASCADE
);

-- Order Details
CREATE TABLE IF NOT EXISTS OrderDetails (
    OrderDetailID INTEGER PRIMARY KEY AUTOINCREMENT,
    OrderID INTEGER NOT NULL,
    ProductID INTEGER NOT NULL,
    Quantity INTEGER NOT NULL CHECK (Quantity > 0),
    Price REAL NOT NULL CHECK (Price >= 0),
    FOREIGN KEY (OrderID) REFERENCES Orders (OrderID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ProductID) REFERENCES Products (ProductID) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Order History
CREATE TABLE IF NOT EXISTS OrderHistory (
    OrderHistoryID INTEGER PRIMARY KEY AUTOINCREMENT,
    OrderID INTEGER NOT NULL,
    Status TEXT NOT NULL,
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (OrderID) REFERENCES Orders (OrderID) ON DELETE CASCADE ON UPDATE CASCADE
);

-- Password should be hashed using an application-level library like bcrypt or Argon2 before storage

-- Add Triggers, Stored Procedures, and other Constraints (omitted for now)

-- Indices for performance
CREATE INDEX IF NOT EXISTS idx_order_userid ON Orders(UserID);
CREATE INDEX IF NOT EXISTS idx_orderdetails_orderid ON OrderDetails(OrderID);
