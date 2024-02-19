-- Create Roles table
CREATE TABLE Roles (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(50) NOT NULL
);

-- Create Users table
CREATE TABLE Users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username VARCHAR(50) UNIQUE NOT NULL,
  password VARCHAR(255) NOT NULL, 
  roleId INTEGER,
  FOREIGN KEY (roleId) REFERENCES Roles(id)
);

-- Create Logs table
CREATE TABLE Logs (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  userId INTEGER,
  action VARCHAR(100),
  timestamp DATETIME,
  FOREIGN KEY (userId) REFERENCES Users(id)
);

-- Create Contacts table
CREATE TABLE Contacts (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(100),
  type VARCHAR(50),
  email VARCHAR(100),
  phone VARCHAR(20)
);

-- Create ContactInfo table
CREATE TABLE ContactInfo (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  contactID INTEGER,
  name VARCHAR(50),
  value VARCHAR(100),
  FOREIGN KEY (contactID) REFERENCES Contacts(id)
);

-- Create Employees table
CREATE TABLE Employees (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(100),
  number VARCHAR(20),
  email VARCHAR(100),
  address TEXT
);

-- Create Suppliers table
CREATE TABLE Suppliers (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(100),
  number VARCHAR(20),
  email VARCHAR(100),
  address TEXT
);

-- Create SuppliersProductInfo table
CREATE TABLE SuppliersProductInfo (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  supplierID INTEGER,
  productTypeId INTEGER,
  code VARCHAR(50),
  FOREIGN KEY (supplierID) REFERENCES Suppliers(id),
  FOREIGN KEY (productTypeId) REFERENCES ProductType(id)
);

-- Create ProductInfo table
CREATE TABLE ProductInfo (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  productTypeId INTEGER,
  name VARCHAR(50),
  value VARCHAR(50),
  FOREIGN KEY (productTypeId) REFERENCES ProductType(id)
);

-- Create ProductType table
CREATE TABLE ProductType (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  code VARCHAR(50),
  barcode VARCHAR(50),
  ukt_zed VARCHAR(50),
  name VARCHAR(100),
  description TEXT,
  category VARCHAR(50),
  lastPrice FLOAT,
  unit VARCHAR(20),
  isImported BOOLEAN
);

-- Create Inventory table
CREATE TABLE Inventory (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  productTypeId INTEGER,
  quantityAvailable INTEGER,
  employeeId INTEGER,
  FOREIGN KEY (productTypeId) REFERENCES ProductType(id),
  FOREIGN KEY (employeeId) REFERENCES Employees(id)
);

-- Create SaleOrders table
CREATE TABLE SaleOrders (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  date DATETIME,
  userId INTEGER,
  contactId INTEGER,
  employeeId INTEGER,
  status VARCHAR(50),
  orderInfoId INTEGER,
  FOREIGN KEY (userId) REFERENCES Users(id),
  FOREIGN KEY (contactId) REFERENCES Contacts(id),
  FOREIGN KEY (employeeId) REFERENCES Employees(id),
  FOREIGN KEY (orderInfoId) REFERENCES OrderInfo(id)
);

-- Create OrderInfo table
CREATE TABLE OrderInfo (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  orderId INTEGER,
  productTypeId INTEGER,
  quantity INTEGER,
  price FLOAT,
  FOREIGN KEY (orderId) REFERENCES SaleOrders(id),
  FOREIGN KEY (productTypeId) REFERENCES ProductType(id)
);

-- Create PurchaseOrders table
CREATE TABLE PurchaseOrders (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  date DATETIME,
  userId INTEGER,
  supplierId INTEGER,
  status VARCHAR(50),
  orderInfoId INTEGER,
  FOREIGN KEY (userId) REFERENCES Users(id),
  FOREIGN KEY (supplierId) REFERENCES Suppliers(id),
  FOREIGN KEY (orderInfoId) REFERENCES OrderInfo(id)
);
