#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "api.h"

#define maxDataLength 8

/* TEST CAN CODE - CLIENT
        MOb1 listens continuously for CAN messages. Based on the message content,
        it drives PORTB high (for a message of 0xFF) or low (for 0x00).
        PORTB should be connected to an LED so that it turns on or off when messages are received.
*/

void handleCANmsg(uint8_t destID, uint8_t msgID, char* msg, uint8_t msgLen) {
    if (*msg==0xFF) {
        PORTB = 0xFF;
    } else {
        PORTB = 0x00;
    }
}

int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    sei(); // enable global interrupts
    initCAN(NODE_demoClient); // initialize CAN bus

    for (;;) {
        // listen for CAN messages forever
    }
}

