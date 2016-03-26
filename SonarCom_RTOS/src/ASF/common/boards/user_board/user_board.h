/**
 * SonarCom board parameters
 *
 */


#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
//#define BOARD_XOSC_HZ          8000000

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
//#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
//#define BOARD_XOSC_STARTUP_US  500000

/************************************************************************/
/* External devices and board parameters                                */
/************************************************************************/

// Board oscillator settings
#define BOARD_FREQ_SLCK_BYPASS		32768U
#define BOARD_FREQ_SLCK_XTAL		BOARD_FREQ_SLCK_BYPASS
#define BOARD_FREQ_MAINCK_BYPASS	12000000U
#define BOARD_FREQ_MAINCK_XTAL		BOARD_FREQ_MAINCK_BYPASS

// Board master clock (OSC) frequency
#define BOARD_MCK					96000000U

// Board main clock (OSC) startup time
#define BOARD_OSC_STARTUP_US		15625

// RTC TWI device
#define BOARD_RTC_TWI_INSTANCE		TWI0
#define BOARD_RTC_DEVICE_ADDRESS	0x0D

// Debug Console UART
#define CONSOLE_UART				UART0
#define CONSOLE_UART_ID				ID_UART0

// GMAC HW configurations
#define BOARD_GMAC_PHY_ADDR			0

// HSMCI pins that shall be configured to access the SD card
#define BOARD_SD_PINS				PINS_HSMCI
#define BOARD_SD_PIN_CD				PIN_HSMCI_CD

// SPI interface definition
#define BOARD_DISP_SPI_BASE			SPI
#define BOARD_DISP_SPI_NPCS			0
#define BOARD_EXT_SPI_BASE			SPI
#define BOARD_EXT_SPI_NPCS			3

// Number of LEDs
#define BOARD_NR_LEDS				4

// Number of ADC
#define BOARD_NR_ADC				4

// Reference voltage for AFEC (mV) */
#define VOLT_REF					(3274)
// The maximal digital value on ADC inputs (mV)
#define MAX_DIGITAL					(4095UL)

/************************************************************************/
/* Pin definitions                                                      */
/************************************************************************/

/**
 * Debug UART Console
 * UART0 pins (UTXD0 and URXD0) definitions: PA10,9
 */
#define PINS_UART0				(PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
#define PINS_UART0_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_UART0_PORT			IOPORT_PIOA

/**
 * GPS UART
 * USART0 pins (TXD0 and RXD0) definitions: PB0,1
 */
#define PINS_USART0				(PIO_PB0C_RXD0 | PIO_PB1C_TXD0)
#define PINS_USART0_FLAGS		(IOPORT_MODE_MUX_C)
#define PINS_USART0_PORT		IOPORT_PIOB

/**
 * RS485 UART
 * USART1 pins (RXD1, TXD1 and RTS1) definitions: PA21,22,24
 */
#define PINS_USART1				(PIO_PA21A_RXD1 | PIO_PA22A_TXD1 | PIO_PA24A_RTS1)
#define PINS_USART1_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART1_PORT		IOPORT_PIOA
#define PIN_USART1_RTS			{PIO_PA24A_RTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RTS_IDX		(PIO_PA24_IDX)
#define PIN_USART1_RTS_FLAGS	(IOPORT_MODE_MUX_A)

/**
 * I2C/TWI
 * TWI0 pins definition: PA3,4
 */
#define PINS_TWI0				(PIO_PA3A_TWD0 | PIO_PA4A_TWCK0)
#define PINS_TWI0_FLAGS			(IOPORT_MODE_MUX_A)
#define PINS_TWI0_PORT			IOPORT_PIOA

/**
 * Display and External SPI
 * SPI pins definition: PA5,11,12,13,14
 */
#define PINS_SPI				(PIO_PA5B_NPCS3 | PIO_PA11A_NPCS0 | PIO_PA12A_MISO | PIO_PA13A_MOSI | PIO_PA14A_SPCK)
#define PINS_SPI_FLAGS			(IOPORT_MODE_MUX_A)
#define PINS_SPI_PORT			IOPORT_PIOA

/**
 * LED #0 (Green)
 * GPIO pin definition: PD24
 */
#define LED0_GPIO				(PIO_PD24_IDX)
//#define LED0_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH
//#define LED0_INACTIVE_LEVEL		IOPORT_PIN_LEVEL_LOW
#define PIN_PWM_LED0_GPIO		LED0_GPIO
#define PIN_PWM_LED0_FLAGS		(IOPORT_MODE_MUX_A)
#define PIN_PWM_LED0_CHANNEL	PWM_CHANNEL_0

/**
 * LED #1 (Red)
 * GPIO pin definition: PD22
 */
#define LED1_GPIO				(PIO_PD22_IDX)
//#define LED1_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH
//#define LED1_INACTIVE_LEVEL		IOPORT_PIN_LEVEL_LOW
#define PIN_PWM_LED1_GPIO		LED1_GPIO
#define PIN_PWM_LED1_FLAGS		(IOPORT_MODE_MUX_A)
#define PIN_PWM_LED1_CHANNEL	PWM_CHANNEL_2

/**
 * LED #2 (Green)
 * GPIO pin definition: PD25
 */
#define LED2_GPIO				(PIO_PD25_IDX)
//#define LED2_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH
//#define LED2_INACTIVE_LEVEL		IOPORT_PIN_LEVEL_LOW
#define PIN_PWM_LED2_GPIO		LED2_GPIO
#define PIN_PWM_LED2_FLAGS		(IOPORT_MODE_MUX_A)
#define PIN_PWM_LED2_CHANNEL	PWM_CHANNEL_1

/**
 * LED #3 (Red)
 * GPIO pin definition: PD21
 */
#define LED3_GPIO				(PIO_PD21_IDX)
//#define LED3_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH
//#define LED3_INACTIVE_LEVEL		IOPORT_PIN_LEVEL_LOW
#define PIN_PWM_LED3_GPIO		LED3_GPIO
#define PIN_PWM_LED3_FLAGS		(IOPORT_MODE_MUX_A)
#define PIN_PWM_LED3_CHANNEL	PWM_CHANNEL_1

#define LED_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH
#define LED_INACTIVE_LEVEL		IOPORT_PIN_LEVEL_LOW

/**
 * Display Reset
 * GPIO pin definition: PD20
 */
#define DISP_RST_GPIO			(PIO_PD20_IDX)
#define DISP_RST_ACTIVE_LEVEL	IOPORT_PIN_LEVEL_LOW
#define DISP_RST_INACTIVE_LEVEL	IOPORT_PIN_LEVEL_HIGH

/**
 * Power relay control
 * GPIO pin definition: PD28
 */
#define RELAY_GPIO				(PIO_PD28_IDX)
#define RELAY_ACTIVE_LEVEL		IOPORT_PIN_LEVEL_HIGH
#define RELAY_INACTIVE_LEVEL	IOPORT_PIN_LEVEL_LOW

/**
 * SD-Card
 * HSMCI pins definition: PA5,8,11,12,13,14
 */
#define PINS_HSMCI				(PIO_PA28C_MCCDA | PIO_PA29C_MCCK | PIO_PA30C_MCDA0 | PIO_PA31C_MCDA1 | PIO_PA26C_MCDA2 | PIO_PA27C_MCDA3)
#define PINS_HSMCI_FLAGS		(IOPORT_MODE_MUX_C)
#define PINS_HSMCI_PORT			IOPORT_PIOC
#define PIN_HSMCI_CD			{PIO_PA8, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}
#define SD_MMC_0_CD_GPIO		(PIO_PA8_IDX)
#define SD_MMC_0_CD_PIO_ID		ID_PIOA
#define SD_MMC_0_CD_FLAGS		(0)
#define SD_MMC_0_CD_DETECT_VALUE (0)
// Number of slot connected on HSMCI interface
#define SD_MMC_HSMCI_MEM_CNT      1
#define SD_MMC_HSMCI_SLOT_0_SIZE  4

/**
 * USB D+/- and Vbus monitoring
 * Pin definition: PB10,11,14
 */
#define PIN_USB_DM				(PIO_PB10)
#define PIN_USB_DP				(PIO_PB11)
#define PIN_USB_VBUS			{PIO_PB14, PIOB, ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define USB_VBUS_FLAGS			(PIO_INPUT | PIO_DEBOUNCE | PIO_IT_EDGE)
#define USB_VBUS_PIN_IRQn		(PIOB_IRQn)
#define USB_VBUS_PIN			(PIO_PB14_IDX)
#define USB_VBUS_PIO_ID			(ID_PIOB)
#define USB_VBUS_PIO_MASK		(PIO_PB14)

/**
 * KSZ8051MNL Ethernet PHY
 * PHY pin definitions: PD0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16
 * PHY Interrupt pin definition: PD29
 * PHY Reset GPIO pin definition: PD17
 */
#define PINS_GMAC_STRAP			(PIO_PD4A_GRXDV | PIO_PD5A_GRX0  | PIO_PD6A_GRX1   | PIO_PD10A_GCRS | \
								 PIO_PD11A_GRX2 | PIO_PD12A_GRX3  | PIO_PD13A_GCOL | PIO_PD14A_GRXCK)
#define PINS_GMAC				(PIO_PD0A_GTXCK | PIO_PD1A_GTXEN | PIO_PD2A_GTX0   | PIO_PD3A_GTX1  | \
								 PIO_PD4A_GRXDV | PIO_PD5A_GRX0  | PIO_PD6A_GRX1   | PIO_PD7A_GRXER | \
								 PIO_PD8A_GMDC  | PIO_PD9A_GMDIO | PIO_PD10A_GCRS  | PIO_PD11A_GRX2 | \
								 PIO_PD12A_GRX3 | PIO_PD13A_GCOL | PIO_PD14A_GRXCK | PIO_PD15A_GTX2 | \
								 PIO_PD16A_GTX3)
#define PINS_GMAC_FLAGS			(IOPORT_MODE_MUX_A)
#define PINS_GMAC_PORT			(IOPORT_PIOD)
#define GMAC_PHY_STRAP			(1<<12)
#define PIN_KSZ8051MNL_INTRP_IDX             PIO_PD29_IDX
#define ETH_RST_GPIO			(PIO_PD17_IDX)
#define ETH_RST_FLAGS			(0)
#define ETH_RST_ACTIVE_LEVEL	IOPORT_PIN_LEVEL_LOW
#define ETH_RST_INACTIVE_LEVEL	IOPORT_PIN_LEVEL_HIGH

/**
 * USB attributes configuration descriptor (bus or self powered,
 * remote wakeup)
 */
#define BOARD_USB_BMATTRIBUTES	USBConfigurationDescriptor_SELFPOWERED_RWAKEUP

// Indicates chip has an UDP Full Speed
#define CHIP_USB_UDP

// Indicates chip has an internal pull-up
#define CHIP_USB_PULLUP_INTERNAL

// Number of USB endpoints
#define CHIP_USB_NUMENDPOINTS	8

// Endpoints max packet size
#define CHIP_USB_ENDPOINTS_MAXPACKETSIZE(i) \
   ((i == 0) ? 64 : \
   ((i == 1) ? 64 : \
   ((i == 2) ? 64 : \
   ((i == 3) ? 64 : \
   ((i == 4) ? 512 : \
   ((i == 5) ? 512 : \
   ((i == 6) ? 64 : \
   ((i == 7) ? 64 : 0 ))))))))

// Endpoints Number of Bank
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