USE TEST;

INSERT INTO Units (UnitName, OKEICode) VALUES ('cm', '0101'), ('m', '1010');
INSERT INTO Credit (CredetSum) VALUES (123), (142), (151);
INSERT INTO Debets (DebetSum) VALUES (51), (41), (14);
INSERT INTO Accounts (DebetID, CreditID) VALUES (1,1), (2,2);
INSERT INTO MaterialPrice (Price) VALUES (500), (501), (502);
INSERT INTO Material (MaterialName, UnitsID, PriceID) VALUES ('Alluminium', 1, 1), ('Ferum', 2, 2);

INSERT INTO Personnel (Position, Name, Signature, SignatureDescription)
VALUES ('Worker', 'Vasilyev Nikolay', 'V', 'Vasilyev'),
       ('Director', 'Kirillov Kirill', 'K', 'Kirillov'),
       ('Accounter', 'Olokuev Okolay', 'O', 'Olokuev');

INSERT INTO Customers (Recipient, Department, Supplier, INN, Address, OKUD, CodeDate, OKPO_1, OKPO_2, KPP, Phone)
VALUES ('R1', 'D1', 'S1', 'INN01', 'Moscow', 'OKUD1', CURRENT_DATE(), 'OKPO_11', 'OKPO_21', 'KPP1', '+88005553535'),
       ('R2', 'D2', 'S2', 'INN02', 'Kiev', 'OKUD2', CURRENT_DATE(), 'OKPO_12', 'OKPO_22', 'KPP2', '+88005553536');