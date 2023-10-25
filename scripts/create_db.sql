-- Create Roles table
CREATE TABLE Roles (
  RoleID INTEGER PRIMARY KEY AUTOINCREMENT,
  Name VARCHAR(50) NOT NULL
);

-- Create Users table
CREATE TABLE Users (
  UserID INTEGER PRIMARY KEY AUTOINCREMENT,
  Username VARCHAR(50) UNIQUE NOT NULL,
  Password VARCHAR(255) NOT NULL, 
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

-- Create SupplierInfo table
CREATE TABLE SupplierInfo (
  SupplierInfoID INTEGER PRIMARY KEY AUTOINCREMENT,
  SupplierID INTEGER,
  Code VARCHAR(50),
  Notes TEXT,
  FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID)
);

-- Create ProductType table
CREATE TABLE ProductType (
  ProductTypeID INTEGER PRIMARY KEY AUTOINCREMENT,
  Code VARCHAR(50),
  Barcode VARCHAR(50),
  UKT_ZED VARCHAR(50),
  Name VARCHAR(100),
  Description TEXT,
  Category VARCHAR(50),
  LastPrice FLOAT,
  Unit VARCHAR(20),
  IsImported BOOLEAN
);

-- Create ProductInfo table
CREATE TABLE ProductInfo (
  ProductInfoID INTEGER PRIMARY KEY AUTOINCREMENT,
  ProductTypeID INTEGER,
  Name VARCHAR(50),
  Value VARCHAR(50),
  FOREIGN KEY (ProductTypeID) REFERENCES ProductType(ProductTypeID)
);

-- Create ProductTypeSupplierInfo table
CREATE TABLE ProductTypeSupplierInfo (
  ProductTypeSupplierInfoID INTEGER PRIMARY KEY AUTOINCREMENT,
  SupplierID INTEGER,
  ProductTypeID INTEGER,
