USE Factory;

SELECT * FROM Customers JOIN Orders;

INSERT INTO Customers (CustomerName, CustomerPhone) VALUE ('Emmanuel', '88001112233');
INSERT INTO Orders (OrderPrice, OrderStartDate, OrderEndDate, CustomerID, TechnologyID) VALUE (1,CURRENT_DATE(), CURRENT_DATE(), 1, 1);

UPDATE Orders SET OrderPrice = 2 WHERE OrderID = LAST_INSERT_ID();

SELECT * FROM Customers JOIN Orders;