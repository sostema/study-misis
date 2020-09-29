DROP DATABASE IF EXISTS Factory;
CREATE DATABASE Factory;
USE Factory;

/*
 Creating Main Tables
 */

#TABLE CREATION

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
 Triggers
 */

CREATE TABLE Discounts
(
    OrderAmount integer UNIQUE NOT NULL,
    Discount    integer        NOT NULL
);

INSERT INTO Discounts
VALUES (0, 500),
       (1, 1000);

CREATE TRIGGER DiscountOnOrder
    BEFORE INSERT
    on Orders
    FOR EACH ROW
BEGIN
    SET @AmountOfOrders = (SELECT COUNT(OrderID) FROM Orders WHERE CustomerID = NEW.CustomerID);
    SET @Discount = (SELECT (Discount)
                     from Discounts
                     Where OrderAmount = (SELECT MAX(OrderAmount) FROM Discounts WHERE OrderAmount <= @AmountOfOrders));
    SET NEW.OrderPrice = (NEW.OrderPrice - IFNULL(@Discount, 0));
end;

#---#

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

#---#

CREATE TABLE ServiceTalons
(
    TalonID integer UNIQUE PRIMARY KEY AUTO_INCREMENT,
    OrderID integer UNIQUE
);

ALTER TABLE ServiceTalons
    ADD FOREIGN KEY (OrderID) REFERENCES Orders (OrderID);

CREATE TRIGGER OnOrderInsert_Talon
    AFTER INSERT
    on Orders
    FOR EACH ROW
begin
    INSERT INTO ServiceTalons (OrderID) VALUE (NEW.OrderID);
end;

CREATE TRIGGER OnOrderDelete_Talon
    BEFORE DELETE
    on Orders
    FOR EACH ROW
begin
    DELETE FROM ServiceTalons WHERE ServiceTalons.OrderID = OLD.OrderID;
end;

#---#

CREATE TRIGGER OnCustomerDelete
    BEFORE DELETE
    on Customers
    FOR EACH ROW
begin
    DELETE FROM Order2Personal WHERE Order2Personal.OrderID in(SELECT (OrderId) FROM Orders WHERE Orders.CustomerID = OLD.CustomerID and Orders.OrderEndDate > CURRENT_DATE());
    DELETE FROM Order2Product WHERE Order2Product.OrderID in(SELECT (OrderId) FROM Orders WHERE Orders.CustomerID = OLD.CustomerID and Orders.OrderEndDate > CURRENT_DATE());
    DELETE FROM Orders WHERE Orders.CustomerID = OLD.CustomerID and Orders.OrderEndDate > CURRENT_DATE();
end;

#---#

CREATE TABLE PreOrders
(
    PreOrderID VARCHAR(64) NOT NULL UNIQUE PRIMARY KEY
);

CREATE TRIGGER OnOrderInsert_PREORDER
    AFTER INSERT
    on Orders
    FOR EACH ROW
begin
    INSERT INTO PreOrders VALUE (CONCAT(CAST(NEW.OrderID as CHAR), '_', CAST(NEW.CustomerID as CHAR)));
end;

CREATE TRIGGER OnOrderDelete_PREORDER
    BEFORE DELETE
    on Orders
    FOR EACH ROW
begin
    DELETE
    FROM PreOrders
    WHERE PreOrderID =
          (CONCAT(CAST(OLD.OrderID as CHAR), '_', CAST(OLD.CustomerID as CHAR)));
end;

#---#

CREATE TRIGGER Product2MaterialInsert
    BEFORE INSERT
    on Product2Material
    for each row
begin
    SET @MaterialAmount = (SELECT (MaterialAmount) FROM Materials WHERE MaterialID = NEW.MaterialID);
    IF (NEW.MaterialQuantity > @MaterialAmount) THEN
        SET @MinPrice = (SELECT MIN(MaterialPrice) FROM SupplierMaterialPrice WHERE MaterialID = NEW.MaterialID) *
                        (NEW.MaterialQuantity - @MaterialAmount);
        SET @msgtext = CONCAT('For production, you would need to buy materials for ', CAST(@MinPrice as CHAR));
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = @msgtext;
    end if;
end;

#---#

Create TABLE OrderProductTime
(
    OrderProductID integer not null auto_increment primary key unique,
    ProductID      integer not null,
    ProductAmount  integer not null,
    DueDay         integer not null,
    DueMonth       integer not null,
    DueYear        integer not null,
    DueDate        date
);

ALTER TABLE OrderProductTime
    ADD FOREIGN KEY (ProductID) REFERENCES Products (ProductID);

CREATE TRIGGER OnOrderProduct
    BEFORE INSERT
    On OrderProductTime
    for each row
begin
    SET NEW.DueDate = STR_TO_DATE(CONCAT(NEW.DueYear, '-', NEW.DueMonth, '-', NEW.DueDay), '%Y-%m-%d');
end;

/*
 Filling up Main Tables
 */

INSERT INTO Departments (DepartmentTitle, DepartmentLocation)
VALUES ('Main Department', 'Russia');

INSERT INTO Equipment (EquipmentName, DepartmentID)
VALUES ('All-in-one Conveyor', 1);

INSERT INTO Customers (CustomerName, CustomerPhone)
VALUES ('Factory Inc.', '88005553535'),
       ('Fabrica Ent.', '89001119999');

INSERT INTO Orders (OrderPrice, OrderStartDate, OrderEndDate, CustomerID)
VALUES (100500, '2019-09-10', '2019-10-10', 1),
       (200100, '2019-09-13', '2019-10-13', 1);

INSERT INTO Suppliers (SupplierName, SupplierPhone)
VALUES ('Supplies Inc.', '88001231234');
INSERT INTO Suppliers (SupplierName, SupplierPhone)
VALUES ('Russia Supply Inc.', '88003214321');

INSERT INTO Products (ProductName, ProductAmount)
VALUES ('aPhone', 12000000),
       ('aPad', 3000000);

INSERT INTO Materials (MaterialName, MaterialAmount)
VALUES ('Processor', 100),
       ('Screen', 100);

INSERT INTO Personal (FirstName, MiddleName, LastName, Age, Occupation, DepartmentID, ManagerID)
VALUES ('Alex', NULL, 'Korner', 45, 'Manager', 1, NULL),
       ('Vladislav', NULL, 'McDorovan', 19, 'Worker', 1, 1),
       ('Michael', NULL, 'Poter', 23, 'Worker', 1, 1),
       ('Dolovan', NULL, 'Richardson', 31, 'Worker', 1, 1);

/*
 Filling up ManyToMany Tables
 */

INSERT INTO SupplierEquipmentPrice (SupplierID, EquipmentID, EquipmentPrice)
VALUES (1, 1, 123000),
       (2, 1, 122999);

INSERT INTO SupplierMaterialPrice (SupplierID, MaterialID, MaterialPrice)
VALUES (1, 1, 50000),
       (2, 1, 55000),
       (1, 2, 35000),
       (2, 2, 32500);

INSERT INTO Supplier2Equipment (SupplierID, EquipmentID, EquipmentPriceID)
VALUES (1, 1, 1),
       (2, 1, 2);

INSERT INTO Supplier2Material (SupplierID, MaterialID, MaterialPriceID, DeliveryTime)
VALUES (1, 1, 1, 20),
       (2, 1, 2, 30),
       (2, 2, 3, 40),
       (2, 2, 4, 50);

INSERT INTO Order2Personal (OrderID, PersonalID)
VALUES (1, 1),
       (1, 2),
       (2, 3),
       (2, 4);

INSERT INTO Order2Product (OrderID, ProductID, ProductAmount)
VALUES (1, 1, 1500),
       (1, 2, 2000),
       (2, 1, 1200),
       (2, 2, 2500);

INSERT INTO OrderProductTime (ProductID, ProductAmount, DueDay, DueMonth, DueYear) VALUE (1, 150, 1, 1, 2020);

INSERT INTO Product2Material (ProductID, MaterialID, MaterialQuantity)
VALUES (1, 1, 1),
       (1, 2, 1),
       (2, 1, 1),
       (2, 2, 1);

#---DELETE CUSTOMER 1---#

DELETE FROM Customers WHERE CustomerID = 1;