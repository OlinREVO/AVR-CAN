#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "api.h"

#define maxDataLength 8

/* TEST CAN CODE - DEMO NODE 1
        Listens for changes on INT0, which is connected to a momentary button.
        When changes occur, an interrupt fires and it sends an appropriate message across the CAN bus.
        If the pin is high, it sends 0xFF. If the pin is low, it sends 0x00.
*/

// TODO: set up 2 more external interrupts

// Set up external interrupts for INT0 for any logical change
int initButton() {
    EICRA = _BV(ISC00);
    EIMSK = _BV(INT0);
    return(0);
}

// TODO: add 2 more ISRs for the other boards (+ the broadcast channel)

// Interrupt routine for External Interrupt 0 (fires when pin 14 changes)
// Reads the value of pin 14 and sends an appropriate CAN message.
ISR(INT0_vect) {
    char cSREG = SREG; //store SREG
    int val = PIND & _BV(PD6);
    uint8_t* msg = (uint8_t*)malloc(1*sizeof(uint8_t));
    if (val) {
        *msg = 0xFF;
    } else {
        *msg = 0x00;
    }
    sendCANmsg(NODE_broadcast, MSG_demoMsg, msg, 1);
    free(msg);
    SREG=cSREG; //restore SREG
}

// TODO: change this method for each of the demo nodes
void handleCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen) {
    uint8_t cmd = *msg;
    PORTB = cmd;
}

int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    DDRD &= ~(_BV(PD6)); // set pin 14 for input

    sei(); // enable global interrupts    
    initCAN(NODE_demoServer); // initialize CAN bus
    initButton(); // intitialize button interrupts

    for (;;) {
        // listen for button presses forever
    }
}

