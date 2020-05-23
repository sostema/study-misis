USE Factory;

DROP ROLE IF EXISTS factory_admin, factory_supplier, factory_customer;
CREATE ROLE factory_admin, factory_supplier, factory_customer;

GRANT ALL ON Factory.* TO factory_admin;

GRANT SELECT, INSERT, UPDATE ON Factory.Suppliers TO factory_supplier;
GRANT SELECT, INSERT, UPDATE ON Factory.Supplier2Material TO factory_supplier;
GRANT SELECT, INSERT, UPDATE ON Factory.Supplier2Equipment TO factory_supplier;
GRANT SELECT, INSERT, UPDATE ON Factory.SupplierMaterialPrice TO factory_supplier;
GRANT SELECT, INSERT, UPDATE ON Factory.SupplierEquipmentPrice TO factory_supplier;
GRANT SELECT ON Factory.Orders TO factory_supplier;

GRANT SELECT, INSERT, UPDATE ON Factory.Orders TO factory_customer;
GRANT SELECT, INSERT, UPDATE ON Factory.Customers TO factory_customer;
GRANT SELECT, INSERT, UPDATE ON Factory.Order2Product TO factory_customer;
GRANT SELECT, INSERT, UPDATE ON Factory.Order2Personal TO factory_customer;

SHOW GRANTS FOR factory_admin;
SHOW GRANTS FOR factory_supplier;
SHOW GRANTS FOR factory_customer;

DROP USER IF EXISTS user_admin@localhost, user_supplier@localhost, user_customer@localhost;
CREATE USER user_admin@localhost IDENTIFIED BY 'adminpass';
CREATE USER user_supplier@localhost IDENTIFIED BY 'supplierpass';
CREATE USER user_customer@localhost IDENTIFIED  BY 'customerpass';

GRANT factory_admin TO user_admin@localhost;
GRANT factory_supplier to user_supplier@localhost;
GRANT factory_customer to user_customer@localhost;

set default role factory_admin to user_admin@localhost;
set default role factory_supplier to user_supplier@localhost;
set default role factory_customer to user_customer@localhost;

SHOW GRANTS FOR user_admin@localhost;
SHOW GRANTS FOR user_supplier@localhost;
SHOW GRANTS FOR user_customer@localhost;

FLUSH PRIVILEGES;