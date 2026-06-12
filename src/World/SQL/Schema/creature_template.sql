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

-- Dumping structure for table content_gquest.creature_template
CREATE TABLE IF NOT EXISTS `creature_template` (
  `guid` bigint(20) DEFAULT NULL,
  `creature_id` int(11) DEFAULT NULL COMMENT 'primary creature ID',
  `creature_name` varchar(50) DEFAULT NULL COMMENT 'primary creature name',
  `creature_subname` varchar(50) DEFAULT NULL COMMENT 'primary creature subname - <Minion of Darkness> etc under the creature name',
  `creature_icon_id` int(11) DEFAULT NULL COMMENT 'in-game mouse over id - displays which UI elements to use  0 = normal 1=elite',
  `creature_type` int(11) DEFAULT NULL COMMENT 'creature classification - 0=normal - 1=elite - 2=boss',
  `display_id` int(11) DEFAULT NULL COMMENT 'Mesh and animation set to use clientside',
  `minlvl` int(11) DEFAULT NULL COMMENT 'min creature level',
  `maxlvl` int(11) DEFAULT NULL COMMENT 'max creature level',
  `loot_pool_id` int(11) DEFAULT NULL COMMENT '0= no loot pool, id of lootpool to use in table lootpool_template',
  `loot_id_00` int(11) DEFAULT NULL COMMENT 'id of lootpool to use in lootpool_template',
  `loot_00_rate` float DEFAULT NULL,
  `basehealth` int(11) DEFAULT NULL COMMENT 'base creature health',
  `base_armor` int(11) DEFAULT NULL COMMENT 'base creature armor',
  `str_attribute` int(11) DEFAULT NULL COMMENT 'strength attribute',
  `sta_attribute` int(11) DEFAULT NULL COMMENT 'stamina attribute',
  `dex_attribute` int(11) DEFAULT NULL COMMENT 'dexterity attribute',
  `int_attribute` int(11) DEFAULT NULL COMMENT 'intelligence attribute',
  `fire_resist` int(11) DEFAULT NULL COMMENT 'creature fire resist attribute',
  `cold_resist` int(11) DEFAULT NULL COMMENT 'creature cold resist attribute',
  `poison_resist` int(11) DEFAULT NULL COMMENT 'creature poison resist attribute',
  `magic_resist` int(11) DEFAULT NULL COMMENT 'creature magic resist attribute',
  `armor_mod` int(11) DEFAULT NULL COMMENT 'armor modifier - multiply armor by this value.',
  `health_mod` float DEFAULT NULL COMMENT 'health modifier - multiply base health by this value.',
  `onuse_ability_00` int(11) DEFAULT NULL COMMENT 'first ability the monster will try to use',
  `onuse_ability_00_cooldown` float DEFAULT NULL COMMENT 'ability cooldown time in ms',
  `onuse_ability_01` int(11) DEFAULT NULL COMMENT 'second ability the monster will try to use',
  `onuse_ability_01_cooldown` float DEFAULT NULL COMMENT 'ability cooldown time in ms',
  `onuse_ability_02` int(11) DEFAULT NULL COMMENT 'third ability the creature will try to use',
  `onuse_ability_02_cooldown` float DEFAULT NULL COMMENT 'ability cooldown time in ms',
  `onuse_ability_03` int(11) DEFAULT NULL COMMENT 'fourth ability the monster will try to use',
  `onuse_ability_03_cooldown` float DEFAULT NULL COMMENT 'ability cooldown time in ms',
  `melee_ability_00` int(11) DEFAULT NULL,
  `melee_ability_00_rate` float DEFAULT NULL COMMENT 'probability of melee use 0=0 percent, 1.0f =100%',
  `melee_ability_01` int(11) DEFAULT NULL,
  `melee_ability_01_rate` float DEFAULT NULL COMMENT 'probability of melee use 0=0 percent, 1.0f =100%',
  `melee_ability_02` int(11) DEFAULT NULL,
  `melee_ability_02_rate` float DEFAULT NULL COMMENT 'probability of melee use 0=0 percent, 1.0f =100%',
  `melee_ability_03` int(11) DEFAULT NULL COMMENT 'Ability to check to use with a melee swing',
  `melee_ability_03_rate` float DEFAULT NULL COMMENT 'probability of melee use 0=0 percent, 1.0f =100%'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

-- Data exporting was unselected.

/*!40103 SET TIME_ZONE=IFNULL(@OLD_TIME_ZONE, 'system') */;
/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IFNULL(@OLD_FOREIGN_KEY_CHECKS, 1) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40111 SET SQL_NOTES=IFNULL(@OLD_SQL_NOTES, 1) */;
