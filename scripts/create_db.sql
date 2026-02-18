PRAGMA foreign_keys = ON;

-- Create Role table
CREATE TABLE Role (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(50) NOT NULL
);

CREATE TABLE User (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username VARCHAR(50) UNIQUE NOT NULL,
  password VARCHAR(255) NOT NULL, 
  roleId INTEGER DEFAULT 1,
  FOREIGN KEY (roleId) REFERENCES Role(id)
);

-- Create Logs table
CREATE TABLE Log (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  userId INTEGER,
  action VARCHAR(100),
  timestamp DATETIME,
  FOREIGN KEY (userId) REFERENCES User(id)
);

-- Create Contacts table
CREATE TABLE Contact (
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
  FOREIGN KEY (contactID) REFERENCES Contact(id)
);

-- Create Employees table
CREATE TABLE Employee (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(100),
  number VARCHAR(20),
  email VARCHAR(100),
  address TEXT
);

-- Create Suppliers table
CREATE TABLE Supplier (
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
  FOREIGN KEY (supplierID) REFERENCES Supplier(id),
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
  name VARCHAR(100),
  description TEXT,
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
  FOREIGN KEY (employeeId) REFERENCES Employee(id)
);

-- Create SaleOrders table
CREATE TABLE SaleOrder (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  date DATETIME,
  userId INTEGER,
  contactId INTEGER,
  employeeId INTEGER,
  status VARCHAR(50),
  FOREIGN KEY (userId) REFERENCES User(id),
  FOREIGN KEY (contactId) REFERENCES Contact(id),
  FOREIGN KEY (employeeId) REFERENCES Employee(id)
);

-- Create SalesOrderRecords table
CREATE TABLE SalesOrderRecord (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  orderId INTEGER,
  productTypeId INTEGER,
  quantity INTEGER,
  price FLOAT,
  FOREIGN KEY (orderId) REFERENCES SaleOrder(id),
  FOREIGN KEY (productTypeId) REFERENCES ProductType(id)
);

-- Create PurchaseOrders table
CREATE TABLE PurchaseOrder (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  date DATETIME,
  userId INTEGER,
  supplierId INTEGER,
  status VARCHAR(50),
  FOREIGN KEY (userId) REFERENCES User(id),
  FOREIGN KEY (supplierId) REFERENCES Supplier(id)
);

-- Create PurchaseOrderRecords table
CREATE TABLE PurchaseOrderRecord (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  orderId INTEGER,
  productTypeId INTEGER,
  quantity INTEGER,
  price FLOAT,
  FOREIGN KEY (orderId) REFERENCES PurchaseOrder(id),
  FOREIGN KEY (productTypeId) REFERENCES ProductType(id)
);


------------------------- INITIAL SETUP ------------------------------------
-- Insert default role
INSERT INTO Role (name) VALUES ('user');
INSERT INTO Role (name) VALUES ('admin');

-- Insert default user with roleId pointing to the default role
-- Password "240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9" is hashed with HEX from "admin123"
INSERT INTO User (username, password, roleId) VALUES ('admin', '240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9', 1); 

-- Insert default employee
INSERT INTO Employee (name, number, email, address) VALUES ('John Doe', '123456789', 'john@example.com', '123 Main St');

-- Insert default supplier
INSERT INTO Supplier (name, number, email, address) VALUES ('Default Supplier', '987654321', 'supplier@example.com', '456 Elm St');

-- Insert default product type
INSERT INTO ProductType (code, barcode, name, description, lastPrice, unit, isImported) 
VALUES ('DEFAULT', '000000000000', 'Default Product', 'Default product description', 0.0, 'unit', 0);

-- Insert default inventory
INSERT INTO Inventory (productTypeId, quantityAvailable, employeeId) VALUES (1, 0, 1); 

-- Insert default contact
INSERT INTO Contact (name, type, email, phone) VALUES ('Default Contact', 'Customer', 'contact@example.com', '123-456-7890');

-- Insert default sale order
INSERT INTO SaleOrder (date, userId, contactId, employeeId, status) VALUES ('2024-02-19', 1, 1, 1, 'Pending'); -- Assuming the default user id, contact id, and employee id are 1

-- Insert default sales order record
INSERT INTO SalesOrderRecord (orderId, productTypeId, quantity, price) VALUES (1, 1, 1, 10.99); 

-- Insert default purchase order
INSERT INTO PurchaseOrder (date, userId, supplierId, status) VALUES ('2024-02-19', 1, 1, 'Pending'); 

-- Insert default purchase order record
INSERT INTO PurchaseOrderRecord (orderId, productTypeId, quantity, price) VALUES (1, 1, 1, 10.99); --Assuming the default purchase order id is 1 and product type id is 1
