USE Factory;

SELECT * FROM Suppliers JOIN Supplier2Material JOIN Supplier2Equipment;

INSERT INTO Suppliers (SupplierName, SupplierPhone) VALUE ('Super Supplier Inc.', '88007775522');
INSERT INTO Supplier2Material VALUE (1, 1, 1, 15);
INSERT INTO Supplier2Equipment VALUE (1, 1, 1);

SELECT * FROM Suppliers JOIN Supplier2Material JOIN Supplier2Equipment;