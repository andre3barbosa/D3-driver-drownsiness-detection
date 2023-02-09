--
-- Database:: `D3_database`
--

-- ----------------------------------------------------------------------------
-- Create database

DROP DATABASE IF EXISTS `D3_database`;
CREATE DATABASE `D3_database`;
USE `D3_database`;

-- --------------------------------------------------------

--
-- Table Structure `User`
--

DROP TABLE IF EXISTS `User`;
CREATE TABLE `User` (
  `USER_ID` int NOT NULL AUTO_INCREMENT,
  `name` char(50) NOT NULL,
  `email` char(50) NOT NULL,
  `password` char(50) NOT NULL,
  PRIMARY KEY (`USER_ID`)
);

-- --------------------------------------------------------

--
-- Table Structure  `Trip`
--

DROP TABLE IF EXISTS `Trip`;
CREATE TABLE `Trip` (
  `TRIP_ID` int NOT NULL AUTO_INCREMENT,
  `trip_duration` float(255,0) NOT NULL,
  `number_of_alerts` int NOT NULL,
  `USER_ID` int DEFAULT NULL,
  PRIMARY KEY (`TRIP_ID`),
  CONSTRAINT `USER_ID` FOREIGN KEY (`USER_ID`) REFERENCES `User` (`USER_ID`)
  			       ON UPDATE CASCADE
);



