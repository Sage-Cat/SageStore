-- Create Roles table
CREATE TABLE Roles (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name VARCHAR(50) NOT NULL
);

CREATE TABLE Users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username VARCHAR(50) UNIQUE NOT NULL,
  password VARCHAR(255) NOT NULL, 
  roleId INTEGER DEFAULT 0,
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



------------------------- INITIAL SETUP ------------------------------------
-- Insert default role
INSERT INTO Roles (name) VALUES ('user');
INSERT INTO Roles (name) VALUES ('admin');

-- Insert default user with roleId pointing to the default role
INSERT INTO Users (username, password, roleId) VALUES ('admin', 'admin123', 1); -- Assuming 'admin' is the default admin user

-- Insert default employee
INSERT INTO Employees (name, number, email, address) VALUES ('John Doe', '123456789', 'john@example.com', '123 Main St');

-- Insert default supplier
INSERT INTO Suppliers (name, number, email, address) VALUES ('Default Supplier', '987654321', 'supplier@example.com', '456 Elm St');

-- Insert default product type
INSERT INTO ProductType (code, barcode, ukt_zed, name, description, category, lastPrice, unit, isImported) 
VALUES ('DEFAULT', '000000000000', 'DEFAULT', 'Default Product', 'Default product description', 'Default category', 0.0, 'unit', 0);

-- Insert default inventory
INSERT INTO Inventory (productTypeId, quantityAvailable, employeeId) VALUES (1, 0, 1); -- Assuming the default product type id is 1 and employee id is 1

-- Insert default contact
INSERT INTO Contacts (name, type, email, phone) VALUES ('Default Contact', 'Customer', 'contact@example.com', '123-456-7890');

-- Insert default sale order
INSERT INTO SaleOrders (date, userId, contactId, employeeId, status, orderInfoId) VALUES ('2024-02-19', 1, 1, 1, 'Pending', 1); -- Assuming the default user id, contact id, and employee id are 1

-- Insert default order info
INSERT INTO OrderInfo (orderId, productTypeId, quantity, price) VALUES (1, 1, 1, 10.99); -- Assuming the default sale order id and product type id are 1

-- Insert default purchase order
INSERT INTO PurchaseOrders (date, userId, supplierId, status, orderInfoId) VALUES ('2024-02-19', 1, 1, 'Pending', 1); -- Assuming the default user id and supplier id are 1
