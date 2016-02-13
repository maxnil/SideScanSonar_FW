/*

 */
 
#ifndef CONF_SPI_MASTER_H_INCLUDED
#define CONF_SPI_MASTER_H_INCLUDED

#include "conf_sonarcom.h"

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

// Display SPI settings
#define CONF_SPI_DISP_DEVICE_ID			0
#define CONF_SPI_DISP_BAUDRATE			CONF_SCOM_SPI_BAUDRATE
#define CONF_SPI_DISP_MODE				SPI_MODE_0
#define CONF_SPI_DISP_BITS_PER_TRANSFER	SPI_CSR_BITS_9_BIT

// External SPI settings
#define CONF_SPI_EXT_DEVICE_ID			3
#define CONF_SPI_EXT_BAUDRATE			CONF_SCOM_SPI_BAUDRATE
#define CONF_SPI_EXT_MODE				SPI_MODE_0
#define CONF_SPI_EXT_BITS_PER_TRANSFER	SPI_CSR_BITS_8_BIT

#define CONF_SPI						SPI

#endif /* CONF_SPI_MASTER_H_INCLUDED */
