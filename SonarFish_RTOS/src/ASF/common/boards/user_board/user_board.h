/**
 * SonarFish board definition
 *
 */


#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

/** Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL		0U
#define BOARD_FREQ_SLCK_BYPASS		32768U
#define BOARD_FREQ_MAINCK_XTAL		0U
#define BOARD_FREQ_MAINCK_BYPASS	12000000U

/** Master clock frequency */
//#define BOARD_MCK                       CHIP_FREQ_CPU_MAX
#define BOARD_MCK					96000000U

/** board main clock xtal startup time */
#define BOARD_OSC_STARTUP_US		15625

/*----------------------------------------------------------------------------*/

/** Reference voltage for ADC (mV) */
#define VOLT_REF					(3000)
/** The maximal digital value on ADC inputs (mV) */
#define MAX_DIGITAL					(4095UL)
#define BOARD_NR_ADC				2
#define ADC_IF_0					ADC_CHANNEL_13
#define ADC_IF_1					ADC_CHANNEL_14

/* Debug UART Console
 * UART0 pins (UTXD0 and URXD0) definitions, PA10,9. */
#define PINS_UART0					( PIO_PA9A_URXD0 \
									| PIO_PA10A_UTXD0)
#define PINS_UART0_FLAGS			(IOPORT_MODE_MUX_A)
#define PINS_UART0_PORT				IOPORT_PIOA
/** Debug Console UART */
#define CONSOLE_UART				UART0
#define CONSOLE_UART_ID				ID_UART0


/* Sensor UART
 * UART1 pins (UTXD1 and URXD1) definitions, PB2,3. */
#define PINS_UART1					( PIO_PB2A_URXD1 \
									| PIO_PB3A_UTXD1)
#define PINS_UART1_FLAGS			(IOPORT_MODE_MUX_A)
#define PINS_UART1_PORT				IOPORT_PIOA


/* RS485 UART
 * USART1 pins (RXD1, TXD1 and RTS1) definitions, PA21,22,24. */
#define PINS_USART1					( PIO_PA21A_RXD1 \
									| PIO_PA22A_TXD1 \
									| PIO_PA24A_RTS1)
#define PINS_USART1_FLAGS			(IOPORT_MODE_MUX_A)
#define PINS_USART1_PORT			IOPORT_PIOA
#define PIN_USART1_RTS				{PIO_PA24A_RTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RTS_IDX			(PIO_PA24_IDX)
#define PIN_USART1_RTS_FLAGS		(IOPORT_MODE_MUX_A)
/* RS485 relate PIN definition. */
#define RS485_USART_ID				ID_USART1
#define RS485_USART					USART1


/* I2C/TWI
 * TWI0 pins definition, PA3,4. */
#define PINS_TWI0					( PIO_PA3A_TWD0 \
									| PIO_PA4A_TWCK0)
#define PINS_TWI0_FLAGS				(IOPORT_MODE_MUX_A)
#define PINS_TWI0_PORT				IOPORT_PIOA
/** TWI instance for Temperature device */
#define BOARD_TEMP_TWI_INSTANCE		TWI0


/* Display and External SPI
 * SPI pins definition, PA5,11,12,13,14. */
#define PINS_SPI					( PIO_PA5B_NPCS3  \
									| PIO_PA11A_NPCS0 \
									| PIO_PA12A_MISO  \
									| PIO_PA13A_MOSI  \
									| PIO_PA14A_SPCK)
#define PINS_SPI_FLAGS				(IOPORT_MODE_MUX_A)
#define PINS_SPI_PORT				IOPORT_PIOA


/* LED #0 (Red)
 * GPIO pin definition, PC28. */
#define LED0_GPIO					(PIO_PC28_IDX)
#define LED0_FLAGS					(0)
#define LED0_ACTIVE_LEVEL			IOPORT_PIN_LEVEL_LOW
#define LED0_INACTIVE_LEVEL			IOPORT_PIN_LEVEL_HIGH
#define LED_RED						LED0_GPIO
#define LED_RED_ACTIVE_LEVEL		LED0_ACTIVE_LEVEL
#define LED_RED_INACTIVE_LEVEL		LED0_INACTIVE_LEVEL

/* LED #1 (blue)
 * GPIO pin definition, PC31. */
#define LED1_GPIO					(PIO_PC31_IDX)
#define LED1_FLAGS					(0)
#define LED1_ACTIVE_LEVEL			IOPORT_PIN_LEVEL_LOW
#define LED1_INACTIVE_LEVEL			IOPORT_PIN_LEVEL_HIGH
#define LED_BLUE					LED1_GPIO
#define LED_BLUE_ACTIVE_LEVEL		LED1_ACTIVE_LEVEL
#define LED_BLUE_INACTIVE_LEVEL		LED1_INACTIVE_LEVEL


/* SD-Card
 * HSMCI pins definition, PA26,27,28,29,30. */
#define PINS_HSMCI					( PIO_PA28C_MCCDA \
									| PIO_PA29C_MCCK  \
									| PIO_PA30C_MCDA0 \
									| PIO_PA31C_MCDA1 \
									| PIO_PA26C_MCDA2 \
									| PIO_PA27C_MCDA3)
#define PINS_HSMCI_FLAGS			(IOPORT_MODE_MUX_C)
#define PINS_HSMCI_PORT				IOPORT_PIOA


/* HSMCI pins definition.
 * Number of slot connected on HSMCI interface */
#define SD_MMC_HSMCI_MEM_CNT		1
#define SD_MMC_HSMCI_SLOT_0_SIZE	4


/** SD-Card detect pin definition. */
#define PIN_HSMCI_CD				{PIO_PC25, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}
#define SD_MMC_0_CD_GPIO			(PIO_PC25_IDX)
#define SD_MMC_0_CD_PIO_ID			ID_PIOC
#define SD_MMC_0_CD_FLAGS			(0)
//Max#define SD_MMC_0_CD_FLAGS        (IOPORT_MODE_PULLUP)
#define SD_MMC_0_CD_DETECT_VALUE	(0)
/** HSMCI pins that shall be configured to access the SD card. */
#define BOARD_SD_PINS				PINS_HSMCI
/** HSMCI Card Detect pin. */
#define BOARD_SD_PIN_CD				PIN_HSMCI_CD


/**
* SPI instance, which can be SPI, SPI0 or SPI1, depends on which SPI
* channel is used.
*/
#define BOARD_EXT_SPI_BASE			SPI
#define BOARD_EXT_SPI_NPCS_0		0
#define BOARD_EXT_SPI_NPCS_1		3


/** USB VBus monitoring pin definition. */
#define PIN_USB_VBUS				{PIO_PB13, PIOB, ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define USB_VBUS_FLAGS				(PIO_INPUT | PIO_DEBOUNCE | PIO_IT_EDGE)
#define USB_VBUS_PIN_IRQn			(PIOB_IRQn)
#define USB_VBUS_PIN				(PIO_PB13_IDX)
#define USB_VBUS_PIO_ID				(ID_PIOB)
#define USB_VBUS_PIO_MASK			(PIO_PB13)
/* USB D- pin (System function) */
#define PIN_USB_DM					{PIO_PB10}
/* USB D+ pin (System function) */
#define PIN_USB_DP					{PIO_PB11}


/* FPGA Memory interface
 * pins definition, PC0-23. */
#define PINS_MEM					( PIO_PC0A_D0    \
									| PIO_PC1A_D1    \
									| PIO_PC2A_D2    \
									| PIO_PC3A_D3    \
									| PIO_PC4A_D4    \
									| PIO_PC5A_D5    \
									| PIO_PC6A_D6    \
									| PIO_PC7A_D7    \
									| PIO_PC8A_NWE   \
									| PIO_PC11A_NRD  \
									| PIO_PC14A_NCS0 \
									| PIO_PC15A_NCS1 \
									| PIO_PC18A_A0   \
									| PIO_PC19A_A1   \
									| PIO_PC20A_A2   \
									| PIO_PC21A_A3   \
									| PIO_PC22A_A4   \
									| PIO_PC23A_A5)
#define PINS_MEM_FLAGS				(IOPORT_MODE_MUX_A)
#define PINS_MEM_PORT				IOPORT_PIOC


/* Ethernet PHY Reset */
//#define ETH_RST_GPIO				(PIO_PD17_IDX)
//#define ETH_RST_FLAGS				(0)
//#define ETH_RST_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_LOW
//#define ETH_RST_INACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH

/*----------------------------------------------------------------------------*/
/**
 * \page usb "ComModule - USB device"
 *
 * \section Definitions
 * - \ref BOARD_USB_BMATTRIBUTES
 * - \ref CHIP_USB_UDP
 * - \ref CHIP_USB_PULLUP_INTERNAL
 * - \ref CHIP_USB_NUMENDPOINTS
 * - \ref CHIP_USB_ENDPOINTS_MAXPACKETSIZE
 * - \ref CHIP_USB_ENDPOINTS_BANKS
 */

/**
 * USB attributes configuration descriptor (bus or self powered,
 * remote wakeup)
 */
#define BOARD_USB_BMATTRIBUTES		USBConfigurationDescriptor_SELFPOWERED_RWAKEUP

/** Indicates chip has an UDP Full Speed. */
#define CHIP_USB_UDP

/** Indicates chip has an internal pull-up. */
#define CHIP_USB_PULLUP_INTERNAL

/** Number of USB endpoints */
#define CHIP_USB_NUMENDPOINTS 8

/** Endpoints max packet size */
#define CHIP_USB_ENDPOINTS_MAXPACKETSIZE(i) \
   ((i == 0) ? 64 : \
   ((i == 1) ? 64 : \
   ((i == 2) ? 64 : \
   ((i == 3) ? 64 : \
   ((i == 4) ? 512 : \
   ((i == 5) ? 512 : \
   ((i == 6) ? 64 : \
   ((i == 7) ? 64 : 0 ))))))))

/** Endpoints Number of Bank */
#define CHIP_USB_ENDPOINTS_BANKS(i) \
   ((i == 0) ? 1 : \
   ((i == 1) ? 2 : \
   ((i == 2) ? 2 : \
   ((i == 3) ? 1 : \
   ((i == 4) ? 2 : \
   ((i == 5) ? 2 : \
   ((i == 6) ? 2 : \
   ((i == 7) ? 2 : 0 ))))))))

#endif // USER_BOARD_H