DROP DATABASE IF EXISTS Factory;
CREATE DATABASE Factory;
USE Factory;

/*
 Creating Main Tables
 */

CREATE TABLE Personal
(
    PersonalID   integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    FirstName    varchar(64)                        NOT NULL,
    MiddleName   varchar(64),
    LastName     varchar(64)                        NOT NULL,
    Age          integer                            NOT NULL,
    Occupation   varchar(64)                        NOT NULL,
    DepartmentID integer                            NOT NULL,
    ManagerID    integer,
    CHECK ( Age >= 18 )
);

CREATE TABLE Departments
(
    DepartmentID       integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    DepartmentTitle    varchar(64)                        NOT NULL,
    DepartmentLocation varchar(64)                        NOT NULL
);

CREATE TABLE Customers
(
    CustomerID    integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    CustomerName  varchar(64),
    CustomerPhone varchar(64)
);

CREATE TABLE Orders
(
    OrderID        integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    OrderPrice     bigint                             NOT NULL,
    OrderStartDate date                               NOT NULL,
    OrderEndDate   date                               NOT NULL,
    CustomerID     integer                            NOT NULL,
    TechnologyID   integer DEFAULT 1                  NOT NULL,
    CHECK ( OrderPrice >= 0 AND OrderEndDate > OrderStartDate )
);

CREATE TABLE Equipment
(
    EquipmentID   integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    EquipmentName varchar(64)                        NOT NULL,
    DepartmentID  integer                            NOT NULL
);

CREATE TABLE Products
(
    ProductID     integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    ProductName   varchar(64)                        NOT NULL,
    ProductAmount integer                            NOT NULL,
    CHECK ( ProductAmount >= 0 )
);

CREATE TABLE Materials
(
    MaterialID     integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    MaterialName   varchar(64)                        NOT NULL,
    MaterialAmount integer                            NOT NULL,
    CHECK ( MaterialAmount >= 0 )
);

CREATE TABLE Suppliers
(
    SupplierID    integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    SupplierName  varchar(64)                        NOT NULL,
    SupplierPhone varchar(64)
);

/*
 Creating Many-To-Many Tables
 */

CREATE TABLE Order2Personal
(
    OrderID    integer NOT NULL,
    PersonalID integer NOT NULL
);

CREATE TABLE Order2Product
(
    OrderID       integer NOT NULL,
    ProductID     integer NOT NULL,
    ProductAmount integer NOT NULL,
    CHECK ( ProductAmount > 0 )
);

CREATE TABLE Product2Material
(
    ProductID        integer NOT NULL,
    MaterialID       integer NOT NULL,
    MaterialQuantity integer NOT NULL
);

CREATE TABLE Supplier2Material
(
    SupplierID      integer NOT NULL,
    MaterialID      integer NOT NULL,
    MaterialPriceID integer NOT NULL,
    DeliveryTime    integer
);

CREATE TABLE Supplier2Equipment
(
    SupplierID       integer NOT NULL,
    EquipmentID      integer NOT NULL,
    EquipmentPriceID integer NOT NULL
);

CREATE TABLE SupplierMaterialPrice
(
    MaterialPriceID integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    SupplierID      integer                            NOT NULL,
    MaterialID      integer                            NOT NULL,
    MaterialPrice   integer                            NOT NULL,
    CHECK ( MaterialPrice > 0 )
);

CREATE TABLE SupplierEquipmentPrice
(
    EquipmentPriceID integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    SupplierID       integer                            NOT NULL,
    EquipmentID      integer                            NOT NULL,
    EquipmentPrice   integer                            NOT NULL,
    CHECK ( EquipmentPrice >= 0 )
);

/*
 Creating Foreign Keys for Main Tables
 */

ALTER TABLE Orders
    ADD FOREIGN KEY (CustomerID) REFERENCES Customers (CustomerID);

ALTER TABLE Personal
    ADD FOREIGN KEY (DepartmentID) REFERENCES Departments (DepartmentID),
    ADD FOREIGN KEY (ManagerID) REFERENCES Personal (PersonalID);

ALTER TABLE Equipment
    ADD FOREIGN KEY (DepartmentID) REFERENCES Departments (DepartmentID);

/*
 Creating Foreign Keys for ManyToMany Tables
 */

ALTER TABLE Order2Personal
    ADD FOREIGN KEY (OrderID) REFERENCES Orders (OrderID),
    ADD FOREIGN KEY (PersonalID) REFERENCES Personal (PersonalID);

ALTER TABLE Order2Product
    ADD FOREIGN KEY (OrderID) REFERENCES Orders (OrderID),
    ADD FOREIGN KEY (ProductID) REFERENCES Products (ProductID);

ALTER TABLE Product2Material
    ADD FOREIGN KEY (ProductID) REFERENCES Products (ProductID),
    ADD FOREIGN KEY (MaterialID) REFERENCES Materials (MaterialID);

ALTER TABLE Supplier2Material
    ADD FOREIGN KEY (SupplierID) REFERENCES Suppliers (SupplierID),
    ADD FOREIGN KEY (MaterialID) REFERENCES Materials (MaterialID),
    ADD FOREIGN KEY (MaterialPriceID) REFERENCES SupplierMaterialPrice (MaterialPriceID);

ALTER TABLE Supplier2Equipment
    ADD FOREIGN KEY (SupplierID) REFERENCES Suppliers (SupplierID),
    ADD FOREIGN KEY (EquipmentID) REFERENCES Equipment (EquipmentID),
    ADD FOREIGN KEY (EquipmentPriceID) REFERENCES SupplierEquipmentPrice (EquipmentPriceID);

ALTER TABLE SupplierMaterialPrice
    ADD FOREIGN KEY (SupplierID) REFERENCES Suppliers (SupplierID),
    ADD FOREIGN KEY (MaterialID) REFERENCES Materials (MaterialID);

ALTER TABLE SupplierEquipmentPrice
    ADD FOREIGN KEY (SupplierID) REFERENCES Suppliers (SupplierID),
    ADD FOREIGN KEY (EquipmentID) REFERENCES Equipment (EquipmentID);

/*
 Additional tables
 */

CREATE TABLE ArchivedOrders
(
    OrderID            integer PRIMARY KEY AUTO_INCREMENT NOT NULL,
    OriginalOrderID    integer                            not null,
    OriginalCustomerID integer                            NOT NULL
);

CREATE TABLE Technologies
(
    TechnologyID       integer UNIQUE AUTO_INCREMENT NOT NULL,
    ImplementationDays integer DEFAULT 0
);
INSERT INTO Technologies (ImplementationDays) VALUE (120);
ALTER TABLE Orders
    ADD FOREIGN KEY (TechnologyID) REFERENCES Technologies (TechnologyID);
CREATE TRIGGER OrderDate
    BEFORE INSERT
    on Orders
    FOR EACH ROW
begin
    set @AllegedDate = DATE_ADD(NEW.OrderStartDate, INTERVAL (SELECT (ImplementationDays)
                                                              FROM Technologies
                                                              WHERE NEW.TechnologyID = Technologies.TechnologyID) DAY);
    set NEW.OrderEndDate = IF(NEW.OrderEndDate < @AllegedDate, @AllegedDate, NEW.OrderEndDate);
end;

CREATE TABLE ServiceTalons
(
    TalonID integer UNIQUE PRIMARY KEY AUTO_INCREMENT,
    OrderID integer UNIQUE
);
ALTER TABLE ServiceTalons
    ADD FOREIGN KEY (OrderID) REFERENCES Orders (OrderID);

CREATE TABLE PreOrders
(
    PreOrderID VARCHAR(64) NOT NULL UNIQUE PRIMARY KEY,
    OrderID int not null
);
ALTER TABLE PreOrders
    ADD FOREIGN KEY (OrderID) REFERENCES Orders (OrderID);

CREATE TABLE OrderJournal
(
    JournalID int PRIMARY KEY UNIQUE AUTO_INCREMENT NOT NULL,
    OrderID int NOT NULL
);
ALTER TABLE OrderJournal
    ADD FOREIGN KEY (OrderID) REFERENCES Orders (OrderID);

CREATE TABLE SupplierJournal
(
    JournalID int PRIMARY KEY UNIQUE AUTO_INCREMENT NOT NULL,
    SupplierID int NOT NULL
);
ALTER TABLE SupplierJournal
    ADD FOREIGN KEY (SupplierID) REFERENCES Suppliers (SupplierID);

CREATE TABLE OrderJournal2Product
(
    JournalID int not null,
    ProductID int not null,
    ProductAmount int not null,
    SummaryDescription varchar(256),
    CHECK ( ProductAmount > 0 )
);
ALTER TABLE OrderJournal2Product
    ADD FOREIGN KEY (JournalID) REFERENCES OrderJournal (JournalID),
    ADD FOREIGN KEY (ProductID) REFERENCES Products (ProductID);

CREATE TABLE SupplierJournal2Material
(
    JournalID int not null,
    MaterialID int not null,
    MaterialAmount int not null,
    TotalCost bigint not null,
    SummaryDescription varchar(256),
    CHECK ( MaterialAmount > 0 )
);
ALTER TABLE SupplierJournal2Material
    ADD FOREIGN KEY (JournalID) REFERENCES SupplierJournal (JournalID),
    ADD FOREIGN KEY (MaterialID) REFERENCES Materials (MaterialID);