USE Factory;

SELECT GetOrderInnovice(1);
SELECT GetMinimalCost(1);
SELECT PersonnelInfo(2, 'Worker');

DROP TABLE IF EXISTS temp1;
CREATE TEMPORARY TABLE temp1
(
    PersonalID integer
);
insert into temp1
values (1), (2);
DROP TABLE IF EXISTS temp2;
CREATE TEMPORARY TABLE temp2
(
    ProductID     integer,
    ProductAmount integer
);
insert into temp2
values (1, 100);
call NewOrder(1, 100500, '2019-09-10', '2019-10-10', 1, 'temp1', 'temp2');

call ShowFullWorkingEnvironment();

call DeleteOrder(1);

call UpdateOrder(3, 20, '2019-09-10', '2019-10-10', 1);

call GetAveragePricesMaterial(1);

call ChangeCustomer(2, 'SuperAmerican Inc.', '+89005553535');