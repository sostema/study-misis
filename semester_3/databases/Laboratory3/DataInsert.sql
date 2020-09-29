USE Factory;

INSERT INTO Departments (DepartmentTitle, DepartmentLocation)
VALUES ('Main Department', 'Russia');

INSERT INTO Equipment (EquipmentName, DepartmentID)
VALUES ('All-in-one Conveyor', 1);

INSERT INTO Customers (CustomerName, CustomerPhone)
VALUES ('Factory Inc.', '88005553535'),
       ('Fabrica Ent.', '89001119999');

INSERT INTO Orders (OrderPrice, OrderStartDate, OrderEndDate, CustomerID)
VALUES (100500, '2019-09-10', '2019-10-10', 1),
       (200100, '2019-09-13', '2019-10-13', 2);

INSERT INTO Suppliers (SupplierName, SupplierPhone)
VALUES ('Supplies Inc.', '88001231234');
INSERT INTO Suppliers (SupplierName, SupplierPhone)
VALUES ('Russia Supply Inc.', '88003214321');

INSERT INTO Products (ProductName, ProductAmount)
VALUES ('aPhone', 1),
       ('aPad', 1);

INSERT INTO Materials (MaterialName, MaterialAmount)
VALUES ('Processor', 100),
       ('Screen', 100);

INSERT INTO Personal (FirstName, MiddleName, LastName, Age, Occupation, DepartmentID, ManagerID)
VALUES ('Alex', NULL, 'Korner', 45, 'Worker', 1, NULL),
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

INSERT INTO Product2Material (ProductID, MaterialID, MaterialQuantity)
VALUES (1, 1, 1),
       (1, 2, 1),
       (2, 1, 1),
       (2, 2, 1);