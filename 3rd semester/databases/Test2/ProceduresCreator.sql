USE TEST;

DROP PROCEDURE IF EXISTS InsertOrder;
CREATE PROCEDURE InsertOrder(OrderID_ int, OrderDate_ DATE, ReciepentName_ varchar(256), TempMaterialID int,
                             Basis_ varchar(256), AdditionalInfo_ varchar(256), PassedName varchar(256),
                             ExecutiveName varchar(256), AcceptedName varchar(256), AcceptedDate_ date,
                             CounterName varchar(256), CounterDate varchar(256))
    MODIFIES SQL DATA
BEGIN
    INSERT INTO Order2Materials (OrderID, MaterialID, MaterialAmount, Price, AccountID)
    SELECT OrderID_, M.MaterialID, TM.MaterialAmount, (MP.Price * TM.MaterialAmount), A.AccountID
    FROM TempMaterial TM
             JOIN Material M on TM.MaterialName = M.MaterialName
    JOIN MaterialPrice MP on M.PriceID = MP.PriceID
    JOIN Accounts A ON A.AccountID = TM.AccountID
    WHERE TM.TempID = TempMaterialID;

    INSERT INTO Orders (OrderID, OrderDate, CustomerID, Basis, AdditionalInfo, PassedOfficerID,
                        ExecutiveOfficerID, AcceptedOfficerID, AcceptedDate, CountingOfficerID, CountingDate)
    SELECT OrderID_,
           OrderDate_,
           C.CustomerID,
           Basis_,
           AdditionalInfo_,
           POID.PersonalID as 'poid',
           EOID.PersonalID as 'eoid',
           AOID.PersonalID as 'aoid',
           AcceptedDate_,
           COID.PersonalID as 'coid',
           CounterDate
    FROM Customers C JOIN Personnel POID on POID.Name = PassedName
    JOIN Personnel EOID on EOID.Name = ExecutiveName
    JOIN Personnel AOID on AOID.Name = AcceptedName
    JOIN Personnel COID on COID.Name = CounterName
    WHERE C.Recipient = ReciepentName_;
end;

DROP PROCEDURE IF EXISTS UpdateOrder;
CREATE PROCEDURE UpdateOrder(OldOrderID_ int, OrderID_ int, OrderDate_ DATE, ReciepentName_ varchar(256), TempMaterialID int,
                             Basis_ varchar(256), AdditionalInfo_ varchar(256), PassedName varchar(256),
                             ExecutiveName varchar(256), AcceptedName varchar(256), AcceptedDate_ date,
                             CounterName varchar(256), CounterDate varchar(256))
    MODIFIES SQL DATA
BEGIN
    UPDATE Orders, Customers C, Personnel POID, Personnel EOID, Personnel AOID, Personnel COID
        SET Orders.OrderID=OrderID_,
           Orders.OrderDate=OrderDate_,
           Orders.CustomerID=C.CustomerID,
           Orders.Basis=Basis_,
           Orders.AdditionalInfo=AdditionalInfo_,
           Orders.PassedOfficerID=POID.PersonalID,
           Orders.ExecutiveOfficerID=EOID.PersonalID,
           Orders.AcceptedOfficerID = AOID.PersonalID,
           Orders.AcceptedDate =AcceptedDate_,
           Orders.CountingOfficerID =COID.PersonalID,
           Orders.CountingDate = CounterDate
    WHERE Orders.OrderID = OldOrderID_ and POID.Name = PassedName and EOID.Name = ExecutiveName and AOID.Name = AcceptedName and COID.Name = CounterName;

    UPDATE Order2Materials O2M, Material M, TempMaterial TM2, MaterialPrice MP2, Accounts A2
    SET
        O2m.OrderID = OrderID_,
        O2M.MaterialID = M.MaterialID,
        O2M.MaterialAmount = (MP2.Price * TM2.MaterialAmount),
        O2M.AccountID = A2.AccountID
    WHERE O2M.OrderID = OldOrderID_ and TM2.TempID = TempMaterialID and M.MaterialName = TM2.MaterialName and A2.AccountID = TM2.AccountID and MP2.PriceID = M.PriceID;
end;

DROP PROCEDURE IF EXISTS DeleteOrder;
CREATE PROCEDURE DeleteOrder(OrderID_ int)
    MODIFIES SQL DATA
BEGIN
    DELETE FROM Order2Materials WHERE OrderID=OrderID_;
    DELETE FROM Orders WHERE OrderID=OrderID_;
end;

DROP PROCEDURE IF EXISTS ShowOrder;
CREATE PROCEDURE ShowOrder(OrderID_ int)
    READS SQL DATA
BEGIN
    SELECT O.*, C.*, O2M.*
    FROM Customers C
        JOIN Orders O ON O.CustomerID = C.CustomerID and O.OrderID = OrderID_
        JOIN Order2Materials O2M on O.OrderID = O2M.OrderID;
end;