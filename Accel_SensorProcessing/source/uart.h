/*
 * uart.h
 *
 *  Created on: 13-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef UART_H_
#define UART_H_

#include "bme.h"
#include "MKL25Z4.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


/*
 * Initialization of UART
 * Baud rate : 115200
 *
 * Parameters:
 *   baud_rate: UART_BAUDRATE
 *
 * Returns:
 *   void
 */
void Init_UART0(uint32_t baud_rate);


/*
 * Function to send string over UART using Enqueue
 *
 * Parameters:
 *   str: String to be sent
 *	 count: length of the string
 *
 * Returns:
 *   void
 */
void Send_String(const void* str, size_t count);


/*
 * Function to Receive the Data from UART
 *
 * Parameters:
 *   str: String to be received from UART
 *	 count: String length
 *
 * Returns:
 *   void
 */
size_t Receive_String(void* str, size_t count);


#endif /* UART_H_ */
