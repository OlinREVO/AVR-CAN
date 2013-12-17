/* ------------------------------- REVO CAN API -------------------------------
 * 
 * Author: Sophie Li, Derek Redfern, Olin College REVO
 * Last modified: Fall 2013
 * Contact: redfern.derek@gmail.com
 *
 * Usage:
 *  1. Include this file in any file where you want to use UART
 *  2. Connect the RX wire on an FTDI cable (usually yellow) to pin 2 on the
 *      microcontroller
 *  3. Call initUART before using these functions
 *  4. Call 
 *
 * Notes:
 *  Calling cli() to disable interrupts will adversely affect these functions
 */

#ifndef UART_H
#define UART_H

/* Standard AVR imports and defines. */

#define F_CPU (1000000L)
#define UART_BAUD (9600L)
#define UART_SAMPLERATE (52UL)
#define BAUD_PRESCALE (((F_CPU / (UART_BAUD * UART_SAMPLERATE))) - 1)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TX_BUFFER_SIZE 200 // maximum size of transmit buffer in bytes

// API Function Prototypes

/* implemented in uart.c
 * turn UART functionality on or off
 */
void initUART(void);
void endUART(void);

/* implemented in uart.c
 * send a string to a computer or other compatible device
 * Sample call: UART_putString("Hello, World!");
 */
int UART_putString(char s[]);

#endif