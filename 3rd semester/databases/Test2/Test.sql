USE TEST;

DELETE FROM TempMaterial WHERE true;

INSERT INTO TempMaterial (TempID, MaterialName, MaterialAmount, AccountID) VALUES (1,'Alluminium', 100, 1), (1, 'Ferum', 25, 1);

call InsertOrder(1, CURRENT_DATE(), 'R1', 1, 'Basis', 'AddInfo', 'Vasilyev Nikolay', 'Vasilyev Nikolay', 'Kirillov Kirill', CURRENT_DATE(), 'Olokuev Okolay', CURRENT_DATE());

call ShowOrder(1);

call UpdateOrder(1, 2, CURRENT_DATE(), 'R1', 1, 'Basis2', 'AddInfo2', 'Kirillov Kirill', 'Vasilyev Nikolay', 'Kirillov Kirill', CURRENT_DATE(), 'Olokuev Okolay', CURRENT_DATE());

call ShowOrder(2);

call DeleteOrder(2);