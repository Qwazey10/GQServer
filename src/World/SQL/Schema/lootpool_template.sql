-- --------------------------------------------------------
-- Host:                         192.168.1.13
-- Server version:               10.11.14-MariaDB-0ubuntu0.24.04.1 - Ubuntu 24.04
-- Server OS:                    debian-linux-gnu
-- HeidiSQL Version:             12.17.0.7270
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;


-- Dumping database structure for content_gquest
CREATE DATABASE IF NOT EXISTS `content_gquest` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */;
USE `content_gquest`;

-- Dumping structure for table content_gquest.lootpool_template
CREATE TABLE IF NOT EXISTS `lootpool_template` (
  `lootpool_id` int(11) DEFAULT NULL,
  `description` varchar(50) DEFAULT NULL,
  `loot_id_00` int(11) DEFAULT NULL,
  `lootdroprate_00` float DEFAULT NULL,
  `mincount_00` int(11) DEFAULT NULL,
  `maxcount_00` int(11) DEFAULT NULL,
  `loot_id_01` int(11) DEFAULT NULL,
  `lootdroprate_01` float DEFAULT NULL,
  `mincount_01` int(11) DEFAULT NULL,
  `maxcount_01` int(11) DEFAULT NULL,
  `loot_id_02` int(11) DEFAULT NULL,
  `lootdroprate_02` float DEFAULT NULL,
  `mincount_02` int(11) DEFAULT NULL,
  `maxcount_02` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- Data exporting was unselected.

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
