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


-- Dumping database structure for character_gquest
CREATE DATABASE IF NOT EXISTS `character_gquest` /*!40100 DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci */;
USE `character_gquest`;

-- Dumping structure for table character_gquest.character
CREATE TABLE IF NOT EXISTS `character` (
  `guid` int(11) DEFAULT NULL COMMENT 'global unique ID',
  `account_id` int(11) DEFAULT NULL COMMENT 'Account ID',
  `character_name` varchar(50) DEFAULT NULL COMMENT 'Character Name',
  `race_id` tinyint(4) DEFAULT NULL COMMENT 'RaceID',
  `class_id` tinyint(4) DEFAULT NULL COMMENT 'ClassID',
  `gender` tinyint(4) DEFAULT NULL COMMENT 'GenderID 0=male 1=female',
  `level` tinyint(4) DEFAULT NULL,
  `xp` int(11) DEFAULT NULL COMMENT 'Experience Points',
  `money` int(11) DEFAULT NULL COMMENT 'Represented in Copper',
  `zone_id` int(11) DEFAULT NULL COMMENT 'zone ID of the character',
  `position_x` float DEFAULT NULL COMMENT 'the X position of the character',
  `position_y` float DEFAULT NULL COMMENT 'The Y Pos of the Character',
  `position_z` float DEFAULT NULL COMMENT 'the z position of the character',
  `position_o` float DEFAULT NULL COMMENT 'Rotation of the character, we only rotate on the yaw axis',
  `transport_x` float DEFAULT NULL COMMENT 'transport local position',
  `transport_y` float DEFAULT NULL COMMENT 'transport local y pos',
  `transport_z` float DEFAULT NULL COMMENT 'transport local z pos',
  `transport_o` float DEFAULT NULL COMMENT 'transport local rot'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci COMMENT='PrimaryCharacterDatabase\r\n';

-- Data exporting was unselected.

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
