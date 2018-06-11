CREATE DATABASE  IF NOT EXISTS `kOferta_test` /*!40100 DEFAULT CHARACTER SET utf8 */;
USE `kOferta_test`;
-- MySQL dump 10.13  Distrib 5.7.22, for Linux (x86_64)
--
-- Host: 127.0.0.1    Database: kOferta_test
-- ------------------------------------------------------
-- Server version	5.5.56-MariaDB

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `addresses`
--

DROP TABLE IF EXISTS `addresses`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `addresses` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `address` text,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `billingTerms`
--

DROP TABLE IF EXISTS `billingTerms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `billingTerms` (
  `id` smallint(6) NOT NULL AUTO_INCREMENT,
  `shortDesc` text,
  `longDesc` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `customers`
--

DROP TABLE IF EXISTS `customers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `customers` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `short` text NOT NULL,
  `full` text NOT NULL,
  `title` text NOT NULL,
  `name` text NOT NULL,
  `surname` text NOT NULL,
  `addressID` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `fk_customers_address_idx` (`addressID`),
  CONSTRAINT `fk_customers_address` FOREIGN KEY (`addressID`) REFERENCES `addresses` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `customersView`
--

DROP TABLE IF EXISTS `customersView`;
/*!50001 DROP VIEW IF EXISTS `customersView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `customersView` AS SELECT 
 1 AS `customerID`,
 1 AS `short`,
 1 AS `full`,
 1 AS `title`,
 1 AS `name`,
 1 AS `surname`,
 1 AS `address`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `deliveryDateTerms`
--

DROP TABLE IF EXISTS `deliveryDateTerms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `deliveryDateTerms` (
  `id` smallint(6) NOT NULL AUTO_INCREMENT,
  `shortDesc` text,
  `longDesc` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `deliveryTerms`
--

DROP TABLE IF EXISTS `deliveryTerms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `deliveryTerms` (
  `id` smallint(6) NOT NULL AUTO_INCREMENT,
  `shortDesc` text,
  `longDesc` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `merchandise`
--

DROP TABLE IF EXISTS `merchandise`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `merchandise` (
  `id` int(10) NOT NULL AUTO_INCREMENT,
  `code` varchar(20) NOT NULL DEFAULT '',
  `description` text,
  `price` decimal(8,2) DEFAULT NULL,
  `unit` enum('szt.','mb.') DEFAULT NULL,
  `validFrom` date NOT NULL DEFAULT '1000-01-01',
  `validTo` date NOT NULL DEFAULT '9999-12-31',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `offerTerms`
--

DROP TABLE IF EXISTS `offerTerms`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `offerTerms` (
  `id` smallint(6) NOT NULL AUTO_INCREMENT,
  `shortDesc` text,
  `longDesc` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `savedOffers`
--

DROP TABLE IF EXISTS `savedOffers`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `savedOffers` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `offerSymbol` varchar(10) NOT NULL,
  `userID` int(11) NOT NULL,
  `customerID` int(11) DEFAULT NULL,
  `offerDate` date NOT NULL,
  `inquiryDate` date DEFAULT NULL,
  `InquiryNumber` text,
  `deliveryTerms` smallint(6) DEFAULT NULL,
  `deliveryDateTerms` smallint(6) DEFAULT NULL,
  `billingTerms` smallint(6) DEFAULT NULL,
  `offerTerms` smallint(6) DEFAULT NULL,
  `remarks` text NOT NULL,
  `dExchangeRate` double DEFAULT NULL,
  `bPrintSpecs` tinyint(4) NOT NULL DEFAULT '1',
  `bPrintRawPrice` tinyint(4) NOT NULL DEFAULT '1',
  `bPrintRawPricePLN` tinyint(4) NOT NULL DEFAULT '1',
  `bPrintDiscount` tinyint(4) NOT NULL DEFAULT '1',
  `bPrintPrice` tinyint(4) NOT NULL DEFAULT '1',
  `bPrintNumber` tinyint(4) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `fk_savedOffers_user_idx` (`userID`),
  KEY `fk_savedOffers_customer_idx` (`customerID`),
  KEY `fk_savedOffers_delivery_idx` (`deliveryTerms`),
  KEY `fk_savedOffers_billing_idx` (`billingTerms`),
  KEY `fk_savedOffers_deliveryDate_idx` (`deliveryDateTerms`),
  KEY `fk_savedOffers_offer_idx` (`offerTerms`),
  CONSTRAINT `fk_savedOffers_offer` FOREIGN KEY (`offerTerms`) REFERENCES `offerTerms` (`id`),
  CONSTRAINT `fk_savedOffers_billing` FOREIGN KEY (`billingTerms`) REFERENCES `billingTerms` (`id`),
  CONSTRAINT `fk_savedOffers_delivery` FOREIGN KEY (`deliveryTerms`) REFERENCES `deliveryTerms` (`id`),
  CONSTRAINT `fk_savedOffers_deliveryDate` FOREIGN KEY (`deliveryDateTerms`) REFERENCES `deliveryDateTerms` (`id`),
  CONSTRAINT `fk_savedOffers_customer` FOREIGN KEY (`customerID`) REFERENCES `customers` (`id`),
  CONSTRAINT `fk_savedOffers_user` FOREIGN KEY (`userID`) REFERENCES `users` (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `savedOffersFullView`
--

DROP TABLE IF EXISTS `savedOffersFullView`;
/*!50001 DROP VIEW IF EXISTS `savedOffersFullView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `savedOffersFullView` AS SELECT 
 1 AS `offerID`,
 1 AS `offerSymbol`,
 1 AS `offerDate`,
 1 AS `inquiryDate`,
 1 AS `inquiryNumber`,
 1 AS `deliveryTerms`,
 1 AS `deliveryDateTerms`,
 1 AS `billingTerms`,
 1 AS `offerTerms`,
 1 AS `remarks`,
 1 AS `dExchangeRate`,
 1 AS `bPrintSpecs`,
 1 AS `bPrintRawPrice`,
 1 AS `bPrintRawPricePLN`,
 1 AS `bPrintDiscount`,
 1 AS `bPrintPrice`,
 1 AS `bPrintNumber`,
 1 AS `customerID`,
 1 AS `short`,
 1 AS `full`,
 1 AS `title`,
 1 AS `name`,
 1 AS `surname`,
 1 AS `address`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `savedOffersMerchandise`
--

DROP TABLE IF EXISTS `savedOffersMerchandise`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `savedOffersMerchandise` (
  `offerID` int(11) NOT NULL,
  `sequenceNumber` int(11) DEFAULT NULL,
  `merchandiseID` int(11) NOT NULL,
  `quantity` double DEFAULT NULL,
  `discount` double DEFAULT NULL,
  KEY `fk_savedOffersMerchandise_offer_idx` (`offerID`),
  KEY `fk_savedOffersMerchandise_1_idx` (`merchandiseID`),
  CONSTRAINT `fk_savedOffersMerchandise_merchandise` FOREIGN KEY (`merchandiseID`) REFERENCES `merchandise` (`id`),
  CONSTRAINT `fk_savedOffersMerchandise_offer` FOREIGN KEY (`offerID`) REFERENCES `savedOffers` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `savedOffersMerchandiseView`
--

DROP TABLE IF EXISTS `savedOffersMerchandiseView`;
/*!50001 DROP VIEW IF EXISTS `savedOffersMerchandiseView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `savedOffersMerchandiseView` AS SELECT 
 1 AS `offerID`,
 1 AS `sequenceNumber`,
 1 AS `quantity`,
 1 AS `discount`,
 1 AS `merchandiseID`,
 1 AS `code`,
 1 AS `description`,
 1 AS `price`,
 1 AS `unit`*/;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `savedOffersView`
--

DROP TABLE IF EXISTS `savedOffersView`;
/*!50001 DROP VIEW IF EXISTS `savedOffersView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE VIEW `savedOffersView` AS SELECT 
 1 AS `offerID`,
 1 AS `offerSymbol`,
 1 AS `offerDate`,
 1 AS `author`,
 1 AS `customerCompany`,
 1 AS `customerName`*/;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `users`
--

DROP TABLE IF EXISTS `users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` text NOT NULL,
  `mail` text NOT NULL,
  `male` tinyint(4) NOT NULL DEFAULT '1',
  `phone` varchar(15) DEFAULT NULL,
  `currentOfferNumber` int(11) NOT NULL DEFAULT '0',
  `currentOfferNumberDate` date NOT NULL DEFAULT '2018-01-01',
  `charForOfferSymbol` char(1) NOT NULL DEFAULT '',
  `password` varchar(45) DEFAULT NULL,
  `salt` varchar(45) DEFAULT NULL,
  `resetPassword` tinyint(1) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  UNIQUE KEY `charForOfferSymbol_UNIQUE` (`charForOfferSymbol`)
) ENGINE=InnoDB AUTO_INCREMENT=1 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping routines for database 'kOferta_test'
--
/*!50003 DROP FUNCTION IF EXISTS `get_new_offer_number` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = '' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`vps162195.ovh.net` FUNCTION `get_new_offer_number`(uid integer) RETURNS int(11)
BEGIN
	DECLARE new_number integer;
	DECLARE old_number integer;
    DECLARE old_date date;
    
    SELECT currentOfferNumber, currentOfferNumberDate
    INTO old_number, old_date
    FROM users
    where users.id = uid;
    
    if year(old_date) < year(curdate()) or month(old_date) < month(curdate()) then
		set new_number=1;
		update users
        set currentOfferNumberDate=curdate(), currentOfferNumber=new_number
        where users.id = uid;
    else
		set new_number=old_number+1;
		update users
        set currentOfferNumber=new_number
        where users.id = uid;
	end if;
    
RETURN new_number;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Final view structure for view `customersView`
--

/*!50001 DROP VIEW IF EXISTS `customersView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `customersView` AS select `customers`.`id` AS `customerID`,`customers`.`short` AS `short`,`customers`.`full` AS `full`,`customers`.`title` AS `title`,`customers`.`name` AS `name`,`customers`.`surname` AS `surname`,`addresses`.`address` AS `address` from (`customers` join `addresses`) where (`customers`.`addressID` = `addresses`.`id`) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `savedOffersFullView`
--

/*!50001 DROP VIEW IF EXISTS `savedOffersFullView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `savedOffersFullView` AS select `savedOffers`.`id` AS `offerID`,`savedOffers`.`offerSymbol` AS `offerSymbol`,`savedOffers`.`offerDate` AS `offerDate`,`savedOffers`.`inquiryDate` AS `inquiryDate`,`savedOffers`.`InquiryNumber` AS `inquiryNumber`,`savedOffers`.`deliveryTerms` AS `deliveryTerms`,`savedOffers`.`deliveryDateTerms` AS `deliveryDateTerms`,`savedOffers`.`billingTerms` AS `billingTerms`,`savedOffers`.`offerTerms` AS `offerTerms`,`savedOffers`.`remarks` AS `remarks`,`savedOffers`.`dExchangeRate` AS `dExchangeRate`,`savedOffers`.`bPrintSpecs` AS `bPrintSpecs`,`savedOffers`.`bPrintRawPrice` AS `bPrintRawPrice`,`savedOffers`.`bPrintRawPricePLN` AS `bPrintRawPricePLN`,`savedOffers`.`bPrintDiscount` AS `bPrintDiscount`,`savedOffers`.`bPrintPrice` AS `bPrintPrice`,`savedOffers`.`bPrintNumber` AS `bPrintNumber`,`customersView`.`customerID` AS `customerID`,`customersView`.`short` AS `short`,`customersView`.`full` AS `full`,`customersView`.`title` AS `title`,`customersView`.`name` AS `name`,`customersView`.`surname` AS `surname`,`customersView`.`address` AS `address` from (`savedOffers` left join `customersView` on((`savedOffers`.`customerID` = `customersView`.`customerID`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `savedOffersMerchandiseView`
--

/*!50001 DROP VIEW IF EXISTS `savedOffersMerchandiseView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `savedOffersMerchandiseView` AS select `savedOffersMerchandise`.`offerID` AS `offerID`,`savedOffersMerchandise`.`sequenceNumber` AS `sequenceNumber`,`savedOffersMerchandise`.`quantity` AS `quantity`,`savedOffersMerchandise`.`discount` AS `discount`,`savedOffersMerchandise`.`merchandiseID` AS `merchandiseID`,`merchandise`.`code` AS `code`,`merchandise`.`description` AS `description`,`merchandise`.`price` AS `price`,`merchandise`.`unit` AS `unit` from (`savedOffersMerchandise` join `merchandise`) where (`savedOffersMerchandise`.`merchandiseID` = `merchandise`.`id`) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `savedOffersView`
--

/*!50001 DROP VIEW IF EXISTS `savedOffersView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`root`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `savedOffersView` AS select `savedOffers`.`id` AS `offerID`,`savedOffers`.`offerSymbol` AS `offerSymbol`,`savedOffers`.`offerDate` AS `offerDate`,`users`.`name` AS `author`,`customers`.`short` AS `customerCompany`,`customers`.`surname` AS `customerName` from ((`savedOffers` join `users` on((`savedOffers`.`userID` = `users`.`id`))) left join `customers` on((`savedOffers`.`customerID` = `customers`.`id`))) */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-06-02 21:10:06
