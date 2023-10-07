-- Create Roles table
CREATE TABLE Roles (
  RoleID INT PRIMARY KEY AUTO_INCREMENT,
  Name VARCHAR(50) NOT NULL
);

-- Create Users table
CREATE TABLE Users (
  UserID INT PRIMARY KEY AUTO_INCREMENT,
  Username VARCHAR(50) UNIQUE NOT NULL,
  Password VARCHAR(255) NOT NULL, -- Assuming encrypted password
  RoleID INT,
  FOREIGN KEY (RoleID) REFERENCES Roles(RoleID)
);

-- Create UserLogs table
CREATE TABLE UserLogs (
  LogID INT PRIMARY KEY AUTO_INCREMENT,
  UserID INT,
  Action VARCHAR(100),
  Timestamp DATETIME,
  FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

-- Create Contacts table
CREATE TABLE Contacts (
  ContactID INT PRIMARY KEY AUTO_INCREMENT,
  Name VARCHAR(100),
  Type VARCHAR(50),
  Email VARCHAR(100),
  Phone VARCHAR(20)
);

-- Create ContactDetails table
CREATE TABLE ContactDetails (
  ContactDetailsID INT PRIMARY KEY AUTO_INCREMENT,
  ContactID INT,
  name VARCHAR(20),
  value VARCHAR(50),
  FOREIGN KEY (ContactID) REFERENCES Contacts(ContactID)
);

-- Create EmployeeRecords table
CREATE TABLE EmployeeRecords (
  EmployeeID INT PRIMARY KEY AUTO_INCREMENT,
  Name VARCHAR(100)
);

-- Create Suppliers table
CREATE TABLE Suppliers (
  SupplierID INT PRIMARY KEY AUTO_INCREMENT,
  Name VARCHAR(100),
  Contact VARCHAR(100),
  Email VARCHAR(100)
);

-- Create ProductType table
CREATE TABLE ProductType (
  TypeID INT PRIMARY KEY AUTO_INCREMENT,
  CatalogueCode VARCHAR(50),
  Barcode VARCHAR(50),
  UKT_ZED VARCHAR(50),
  Unit VARCHAR(20),
  IsImported BOOLEAN,
  SupplierID INT,
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID)
);

-- Create Products table
CREATE TABLE Products (
  ProductID INT PRIMARY KEY AUTO_INCREMENT,
  Name VARCHAR(100),
  Description TEXT,
  Category VARCHAR(50),
  ProductTypeID INT,
  EmployeeRecordID INT,
  FOREIGN KEY (ProductTypeID) REFERENCES ProductType(TypeID),
  FOREIGN KEY (EmployeeRecordID) REFERENCES EmployeeRecords(EmployeeID)
);

-- Create Inventory table
CREATE TABLE Inventory (
  InventoryID INT PRIMARY KEY AUTO_INCREMENT,
  ProductID INT,
  UserID INT,
  QuantityAvailable INT,
  FOREIGN KEY (ProductID) REFERENCES Products(ProductID),
  FOREIGN KEY (UserID) REFERENCES Users(UserID)
);

-- Create Orders table
CREATE TABLE Orders (
  OrderID INT PRIMARY KEY AUTO_INCREMENT,
  Date DATETIME,
  UserID INT,
  CustomerID INT,
  Status VARCHAR(50),
  Type VARCHAR(50),
  FOREIGN KEY (UserID) REFERENCES Users(UserID),
  FOREIGN KEY (CustomerID) REFERENCES Contacts(ContactID)
);

-- Create OrderDetails table
CREATE TABLE OrderDetails (
  OrderDetailID INT PRIMARY KEY AUTO_INCREMENT,
  OrderID INT,
  ProductID INT,
  Quantity INT,
  Price FLOAT,
  FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
  FOREIGN KEY (ProductID) REFERENCES Products(ProductID)
);

-- Create PurchaseOrders table
CREATE TABLE PurchaseOrders (
  PurchaseOrderID INT PRIMARY KEY AUTO_INCREMENT,
  Date DATETIME,
  UserID INT,
  SupplierID INT,
  Status VARCHAR(50),
  FOREIGN KEY (UserID) REFERENCES Users(UserID),
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID)
);
