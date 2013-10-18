/* ------------------------------- REVO CAN API -------------------------------
 * 
 * Author: Derek Redfern, Heather Boortz, Olin College REVO
 * Last modified: Fall 2013
 * Contact: redfern.derek@gmail.com
 *
 * Usage:
 *  1. Include this file in any file where you want to use the API functions
 *  2. Define a method called handleCANmsg according to the prototype below.
 *     This method will be called when a CAN message is received.
 *  2. Call initCAN before entering your main loop. Include the ID of the
 *     current node in the call. (Contact the CAN lead if your node does not
 *     have an ID assignment below.)
 *  3. Use sendCANmsg to send a message to another node!
 *
 * Notes:
 *  Calling cli() to disable interrupts will adversely affect these APIs
 */

#ifndef CAN_API_H
#define CAN_API_H

/* Standard AVR imports and defines. */

#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>


/* Definitions for node IDs and message IDs.
/* Please feel free to add IDs as necessary, but make sure
 *  to synchronize this file between all nodes
 *
 * NODE: max of 5 bits
 * MSG: max of 6 bihs
 */
#define NODE_watchdog       0;
#define NODE_bms            1;
#define NODE_speedometer    2;
#define NODE_halleffect     3;
#define NODE_sdlogger       4;

#define MSG_critical        0;
#define MSG_warning         1;
#define MSG_speed           2;
#define MSG_voltagelow		3;
#define MSG_shunting		4;
#define MSG_shutdown        5;
#define MSG_data_other      6;

//#define MSG_batteryLevel    3;
//#define MSG_batteryUsage    4;

// API Function Prototypes

/* implemented in api.c
 * must be called before CAN can be used
 * Sample call: initCAN(NODE_speedometer);
 */
int initCAN(int nodeID);

/* implemented in api.c
 * Sample call: sendCANmsg(NODE_watchdog,MSG_critical,data,dataLen);
 */
int sendCANmsg(uint8_t destID, uint8_t msgID, char* msg, uint8_t msgLen);

/* must be implemented by user. A sample implementation is included in
 *  a block comment in api.c
 * make sure not to do any heavy computation in this method, since it
 *  will be called from an ISR and will delay your main loop
 */
void handleCANmsg(uint8_t destID, uint8_t msgID, char* msg, uint8_t msgLen);

#endif