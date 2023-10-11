-- Create Roles table
CREATE TABLE Roles (
  RoleID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(50) NOT NULL
);

-- Create Users table
CREATE TABLE Users (
  UserID INTEGER PRIMARY KEY AUTOINCREMENT,
  Username VARCHAR(50) UNIQUE NOT NULL,
  Password VARCHAR(255) NOT NULL, -- Assuming encrypted password
  RoleID INTEGER,
  FOREIGN KEY (RoleID) REFERENCES Roles(RoleID)
);

-- Create UserLogs table
CREATE TABLE UserLogs (
  LogID INTEGER PRIMARY KEY AUTOINCREMENT,
  UserID INTEGER,
  Action VARCHAR(100),
  Timestamp DATETIME,
  FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

-- Create Contacts table
CREATE TABLE Contacts (
  ContactID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(100),
  Type VARCHAR(50),
  Email VARCHAR(100),
  Phone VARCHAR(20)
);

-- Create ContactDetails table
CREATE TABLE ContactDetails (
  ContactDetailsID INTEGER PRIMARY KEY AUTOINCREMENT,
  ContactID INTEGER,
  name VARCHAR(20),
  value VARCHAR(50),
  FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

-- Create EmployeeRecords table
CREATE TABLE EmployeeRecords (
  EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(100)
);

-- Create Suppliers table
CREATE TABLE Suppliers (
  SupplierID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(100),
  Contact VARCHAR(100),
  Email VARCHAR(100)
);

-- Create ProductType table
CREATE TABLE ProductType (
  TypeID INTEGER PRIMARY KEY AUTOINCREMENT,
  CatalogueCode VARCHAR(50),
  Barcode VARCHAR(50),
  UKT_ZED VARCHAR(50),
  Unit VARCHAR(20),
  IsImported BOOLEAN,
  SupplierID INTEGER,
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID)
);

-- Create Products table
CREATE TABLE Products (
  ProductID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(100),
  Description TEXT,
  Category VARCHAR(50),
  ProductTypeID INTEGER,
  EmployeeRecordID INTEGER,
  FOREIGN KEY (ProductTypeID) REFERENCES ProductType(TypeID),
  FOREIGN KEY (EmployeeRecordID) REFERENCES EmployeeRecords(EmployeeID)
);

-- Create Inventory table
CREATE TABLE Inventory (
  InventoryID INTEGER PRIMARY KEY AUTOINCREMENT,
  ProductID INTEGER,
  UserID INTEGER,
  QuantityAvailable INTEGER,
  FOREIGN KEY (ProductID) REFERENCES Products(ProductID),
  FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

-- Create Orders table
CREATE TABLE Orders (
  OrderID INTEGER PRIMARY KEY AUTOINCREMENT,
  Date DATETIME,
  UserID INTEGER,
  CustomerID INTEGER,
  Status VARCHAR(50),
  Type VARCHAR(50),
  FOREIGN KEY (UserID) REFERENCES Users(UserID),
  FOREIGN KEY (CustomerID) REFERENCES Contacts(ContactID)
);

-- Create OrderDetails table
CREATE TABLE OrderDetails (
  OrderDetailID INTEGER PRIMARY KEY AUTOINCREMENT,
  OrderID INTEGER,
  ProductID INTEGER,
  Quantity INTEGER,
  Price FLOAT,
  FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
  FOREIGN KEY (ProductID) REFERENCES Products(ProductID)
);

-- Create PurchaseOrders table
CREATE TABLE PurchaseOrders (
  PurchaseOrderID INTEGER PRIMARY KEY AUTOINCREMENT,
  Date DATETIME,
  UserID INTEGER,
  SupplierID INTEGER,
  Status VARCHAR(50),
  FOREIGN KEY (UserID) REFERENCES Users(UserID),
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID)
);
