DROP DATABASE IF EXISTS TEST;
CREATE DATABASE TEST;
USE TEST;

CREATE TABLE Personal
(
    PersonalID          integer not null primary key auto_increment unique,
    PersonalName        varchar(64),
    PersonalProficiency integer default 0
);

CREATE TABLE Schedule
(
    PersonalID  integer not null primary key unique,
    ScheduleNON DATE    not null
);

ALTER TABLE Schedule
    ADD FOREIGN KEY (PersonalID) references Personal (PersonalID);

CREATE TABLE Shift
(
    ShiftDay    date primary key unique,
    Preferences integer default 0,
    PersonalID  integer
);

ALTER TABLE Shift
    ADD FOREIGN KEY (PersonalID) references Personal (PersonalID);

CREATE TRIGGER OnShiftInsert
    BEFORE INSERT
    on Shift
    FOR EACH ROW
BEGIN
    SET @ChosenPersonal = (SELECT (PersonalID)
                           FROM Personal
                           WHERE (
                                         PersonalID NOT IN (SELECT (PersonalID)
                                              FROM (SELECT * FROM Schedule WHERE Schedule.ScheduleNON = NEW.ShiftDay) as `S*`)
                                         AND
                                         PersonalProficiency >= NEW.Preferences
    ) LIMIT 1);
    IF (@ChosenPersonal IS NOT NULL) THEN
        SET NEW.PersonalID = @ChosenPersonal;
    ELSE
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'NON AVALIABLE PERSONAL';
    end if;
END;

INSERT INTO Personal (PersonalName, PersonalProficiency)
VALUES ('Ivan', 0),
       ('Kirill', 1),
       ('Michael', 0),
       ('Mikhail', 3);

INSERT INTO Schedule (PersonalID, ScheduleNON)
VALUES (2, '2019-11-11'), (3, '2019-11-12');
SELECT *
FROM Shift;
INSERT INTO Shift (ShiftDay, Preferences)
VALUES ('2019-11-10', 0),
       ('2019-11-11', 1),
       ('2019-11-12', 0),
       ('2019-11-13', 2),
       ('2019-11-14', 3),
       ('2019-11-15', 0),
       ('2019-11-16', 0);
SELECT *
FROM Shift;
INSERT INTO Shift (ShiftDay, Preferences)
VALUES ('2019-11-17', 5);

