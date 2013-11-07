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
 * Please feel free to add IDs as necessary, but make sure
 *  to synchronize this file between all nodes
 *
 * NODE: max of 5 bits
 * MSG: max of 6 bits
 */
#define NODE_watchdog       ((uint8_t) 0x0)
#define NODE_bms            ((uint8_t) 0x1)
#define NODE_broadcast      ((uint8_t) 0x2) // All nodes listen for a broadcast
#define NODE_speedometer    ((uint8_t) 0x3)
#define NODE_halleffect     ((uint8_t) 0x4)
#define NODE_sdlogger       ((uint8_t) 0x5)
#define NODE_demoClient     ((uint8_t) 0xA)
#define NODE_demoServer     ((uint8_t) 0xB)
#define NODE_demoNode1      ((uint8_t) 0xC)
#define NODE_demoNode2      ((uint8_t) 0xD)
#define NODE_demoNode3      ((uint8_t) 0xE)

#define MSG_critical        ((uint8_t) 0x0)
#define MSG_warning         ((uint8_t) 0x1)
#define MSG_speed           ((uint8_t) 0x2)
#define MSG_voltagelow		((uint8_t) 0x3)
#define MSG_shunting		((uint8_t) 0x4)
#define MSG_shutdown        ((uint8_t) 0x5)
#define MSG_data_other      ((uint8_t) 0x6)
#define MSG_demoMsg         ((uint8_t) 0xA)

//#define MSG_batteryLevel    3;
//#define MSG_batteryUsage    4;

// API Function Prototypes

/* implemented in api.c
 * must be called before CAN can be used
 * Sample call: initCAN(NODE_speedometer);
 */
int initCAN(uint8_t nodeID);

/* implemented in api.c
 * Sample call: sendCANmsg(NODE_watchdog,MSG_critical,data,dataLen);
 */
int sendCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen);

/* must be implemented by user. A sample implementation is included in
 *  a block comment in api.c
 * make sure not to do any heavy computation in this method, since it
 *  will be called from an ISR and will delay your main loop
 */
void handleCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen);

#endif