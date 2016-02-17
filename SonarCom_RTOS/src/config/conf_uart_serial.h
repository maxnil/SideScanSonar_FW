/**
 * \file
 *
 * \brief Serial USART service configuration.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_USART_SERIAL_H
#define CONF_USART_SERIAL_H

/** Debug UART setting */
#define CONF_UART				CONSOLE_UART
#define CONSOLE_UART_BAUDRATE   115200
#define CONSOLE_UART_PARITY     UART_MR_PAR_NO

/** GPS UART setting */
#define CONF_GPS_BAUDRATE		CONF_SCOM_GPS_USART_BAUDRATE
#define CONF_GPS_ID_USART		ID_USART0
#define CONF_GPS_USART			USART0

/** RS485 UART setting */
#define CONF_RS485_BAUDRATE		CONF_SCOM_RS485_USART_BAUDRATE
#define CONF_RS485_ID_USART		ID_USART1
#define CONF_RS485_USART		USART1
//#define RS485_USART_Handler		USART1_Handler
//#define CONF_RS485_IRQn			USART1_IRQn
//#define CONF_RS485_PDC			PDC_USART1

#endif /* CONF_USART_SERIAL_H_INCLUDED */
