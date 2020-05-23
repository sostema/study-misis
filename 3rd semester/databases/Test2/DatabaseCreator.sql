DROP DATABASE IF EXISTS TEST;
CREATE DATABASE TEST;
USE TEST;

CREATE TABLE Orders (
    OrderID integer not null auto_increment primary key,
    OrderDate date,
    CustomerID int,
    Basis varchar(256),
    AdditionalInfo varchar(256),
    PassedOfficerID integer,
    ExecutiveOfficerID integer,
    AcceptedOfficerID integer,
    AcceptedDate date,
    CountingOfficerID integer,
    CountingDate date
);

CREATE TABLE Order2Materials (
    OrderID int,
    MaterialID int,
    MaterialAmount int,
    Price int,
    AccountID int
);

CREATE TABLE Customers (
    CustomerID integer not null auto_increment primary key,
    Recipient varchar(256),
    Department varchar(256),
    Supplier varchar(256),
    INN varchar(256),
    Address varchar(256),
    OKUD varchar(256),
    CodeDate date,
    OKPO_1 varchar(256),
    OKPO_2 varchar(256),
    KPP varchar(256),
    Phone varchar(256)
);

CREATE TABLE Personnel (
    PersonalID integer not null auto_increment primary key,
    Name varchar(256),
    Position varchar(256),
    Signature varchar(256),
    SignatureDescription varchar(256)
);

CREATE TABLE Material (
    MaterialID integer not null auto_increment primary key,
    MaterialName varchar(256),
    UnitsID integer,
    PriceID integer
);

CREATE TABLE Units (
    UnitID integer not null auto_increment primary key,
    UnitName varchar(256),
    OKEICode varchar(256)
);

CREATE TABLE MaterialPrice (
    PriceID integer not null auto_increment primary key,
    Price integer
);

CREATE TABLE Accounts (
    AccountID integer not null auto_increment primary key,
    DebetID int,
    CreditID int
);

CREATE TABLE Debets (
    DebetID integer not null auto_increment primary key,
    DebetSum int
);

CREATE TABLE Credit (
    CreditID integer not null auto_increment primary key,
    CredetSum int
);

CREATE TABLE TempMaterial
(
    TempID         integer,
    MaterialName   varchar(256),
    MaterialAmount int,
    AccountID int
);