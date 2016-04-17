/*

 */
 
#ifndef CONF_SPI_MASTER_H_INCLUDED
#define CONF_SPI_MASTER_H_INCLUDED

#include "conf_sonarfish.h"

/* Possibility to change low-level configurations here. */

//! Default Configuration of SPI Master Delay BCS
// #define CONFIG_SPI_MASTER_DELAY_BCS            0

//! Default Configuration of SPI Master Bits per Transfer Definition
// #define CONFIG_SPI_MASTER_BITS_PER_TRANSFER    8

//! Default Configuration of SPI Master Delay BCT
// #define CONFIG_SPI_MASTER_DELAY_BCT            0

//! Default Configuration of SPI Master Delay BS
// #define CONFIG_SPI_MASTER_DELAY_BS             0

//! Default Configuration of SPI Master Dummy Field
// #define CONFIG_SPI_MASTER_DUMMY                0xFF

// External SPI #0 settings
#define CONF_SPI_EXT_0_DEVICE_ID			0
#define CONF_SPI_EXT_0_BAUDRATE				CONF_SFISH_SPI_BAUDRATE
#define CONF_SPI_EXT_0_MODE					SPI_MODE_0
#define CONF_SPI_EXT_0_BITS_PER_TRANSFER	SPI_CSR_BITS_8_BIT

// External SPI #1 settings
#define CONF_SPI_EXT_1_DEVICE_ID			3
#define CONF_SPI_EXT_1_BAUDRATE				CONF_SFISH_SPI_BAUDRATE
#define CONF_SPI_EXT_1_MODE					SPI_MODE_0
#define CONF_SPI_EXT_1_BITS_PER_TRANSFER	SPI_CSR_BITS_8_BIT

#define CONF_SPI						SPI

#endif /* CONF_SPI_MASTER_H_INCLUDED */
