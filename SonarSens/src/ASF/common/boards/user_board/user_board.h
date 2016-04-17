/**
 * \file
 *
 * \brief User board definition template
 *
 */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */

#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
//#define BOARD_XOSC_HZ          12000000

// External oscillator type.
//!< External clock signal
//Max #define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
//Max #define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
//#define BOARD_XOSC_STARTUP_US  500000

/* ! LED ON SensorBoard MN006. */
#define LED_GREEN_GPIO       IOPORT_CREATE_PIN(PORTB, 1)
#define LED_YELLOW_GPIO      IOPORT_CREATE_PIN(PORTB, 2)

/* ! Status signals. */
#define SENSORS_ACC_DRDY     IOPORT_CREATE_PIN(PORTD, 3)  // Accelerometer Data Ready as input
#define SENSORS_COMP_DRDY    IOPORT_CREATE_PIN(PORTC, 1)  // Compass Data Ready as input
#define SENSORS_GYRO_DRDY    IOPORT_CREATE_PIN(PORTC, 2)  // Gyro Data Ready as input
#define SENSORS_IRTEMP_DRDY  IOPORT_CREATE_PIN(PORTD, 4)  // IR Temperature Data Ready as input

/*! \name I2C Connections.
 */
//! @{
#define I2C             &TWCR
#define I2C_SDA         IOPORT_CREATE_PIN(PORTC,4)  // I2C SDA as inout
#define I2C_SCL         IOPORT_CREATE_PIN(PORTC,5)  // I2C SCL as inout
//! @}

/*! \name UART Connections.
 */
//! @{
#define UART            &SPCR
#define UART_RX         IOPORT_CREATE_PIN(PORTD,0)  // UART RX as input
#define UART_TX         IOPORT_CREATE_PIN(PORTD,1)  // UART TX as output
//! @}

/*! \name SPI Connections of the Compass, Gyro and Accelerometer.
 */
//! @{
#define SENSORS_SPI      &SPCR
#define SENSORS_ACC_CS   IOPORT_CREATE_PIN(PORTD,2)  // Accelerometer CS as output
#define SENSORS_COMP_CS  IOPORT_CREATE_PIN(PORTC,0)  // Compass CS as output
#define SENSORS_GYRO_CS  IOPORT_CREATE_PIN(PORTC,3)  // Gyro CS as output
#define SENSORS_SCK      IOPORT_CREATE_PIN(PORTB,5)  // SCK as output
#define SENSORS_MOSI     IOPORT_CREATE_PIN(PORTB,3)  // MOSI as output
#define SENSORS_MISO     IOPORT_CREATE_PIN(PORTB,4)  // MISO as input
//! @}

/*! \name 'bit-bang' SPI Connections of the Pressure Sensor.
 */
//! @{
#define PRESSURE_SCK     IOPORT_CREATE_PIN(PORTD,5)  // SCK as output
#define PRESSURE_MOSI    IOPORT_CREATE_PIN(PORTD,7)  // MOSI as output
#define PRESSURE_MISO    IOPORT_CREATE_PIN(PORTD,6)  // MISO as input
//! @}

#endif // USER_BOARD_H
