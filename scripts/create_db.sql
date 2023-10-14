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

-- Create ContactInfo table
CREATE TABLE ContactInfo (
  ContactInfoID INTEGER PRIMARY KEY AUTOINCREMENT,
  ContactID INTEGER,
  Name VARCHAR(50),
  Value VARCHAR(100),
  FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

-- Create Employees table
CREATE TABLE Employees (
  EmployeeID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(100),
  Number VARCHAR(20),
  Email VARCHAR(100),
  Address TEXT
);

-- Create Suppliers table
CREATE TABLE Suppliers (
  SupplierID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(100),
  Number VARCHAR(20),
  Email VARCHAR(100),
  Address TEXT
);

-- Create ProductType table
CREATE TABLE ProductType (
  ProductTypeID INTEGER PRIMARY KEY AUTOINCREMENT,
  CatalogueCode VARCHAR(50),
  Barcode VARCHAR(50),
  UKT_ZED VARCHAR(50),
  Name VARCHAR(100),
  Description TEXT,
  Category VARCHAR(50),
  LastPrice FLOAT,
  Unit VARCHAR(20),
  IsImported BOOLEAN
);

-- Create ProductTypeSupplierInfo table
CREATE TABLE ProductTypeSupplierInfo (
  ProductTypeSupplierInfoID INTEGER PRIMARY KEY AUTOINCREMENT,
  ProductTypeID INTEGER,
  Code VARCHAR(50),
  SupplierID INTEGER,
  FOREIGN KEY (ProductTypeID) REFERENCES ProductType(ProductTypeID),
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID)
);

-- Create Inventory table
CREATE TABLE Inventory (
  InventoryID INTEGER PRIMARY KEY AUTOINCREMENT,
  ProductTypeID INTEGER,
  QuantityAvailable INTEGER,
  EmployeeID INTEGER,
  FOREIGN KEY (ProductTypeID) REFERENCES ProductType(ProductTypeID),
  FOREIGN KEY (EmployeeID) REFERENCES Employees(EmployeeID)
);

-- Create SaleOrders table
CREATE TABLE SaleOrders (
  OrderID INTEGER PRIMARY KEY AUTOINCREMENT,
  Date DATETIME,
  UserID INTEGER,
  CustomerID INTEGER,
  Status VARCHAR(50),
  OrderInfoID INTEGER,
  FOREIGN KEY (UserID) REFERENCES Users(UserID),
  FOREIGN KEY (CustomerID) REFERENCES Contacts(ContactID),
  FOREIGN KEY (OrderInfoID) REFERENCES OrderInfo(OrderInfoID)
);

-- Create OrderInfo table
CREATE TABLE OrderInfo (
  OrderInfoID INTEGER PRIMARY KEY AUTOINCREMENT,
  OrderID INTEGER,
  ProductTypeID INTEGER,
  Quantity INTEGER,
  Price FLOAT,
  FOREIGN KEY (OrderID) REFERENCES SaleOrders(OrderID),
  FOREIGN KEY (ProductTypeID) REFERENCES ProductType(ProductTypeID)
);

-- Create PurchaseOrders table
CREATE TABLE PurchaseOrders (
  PurchaseOrderID INTEGER PRIMARY KEY AUTOINCREMENT,
  Date DATETIME,
  UserID INTEGER,
  SupplierID INTEGER,
  Status VARCHAR(50),
  OrderInfoID INTEGER,
  FOREIGN KEY (UserID) REFERENCES Users(UserID),
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID),
  FOREIGN KEY (OrderInfoID) REFERENCES OrderInfo(OrderInfoID)
);
