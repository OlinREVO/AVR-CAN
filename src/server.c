#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "api.h"

#define maxDataLength 8

/* TEST CAN CODE - SERVER
        Listens for changes on INT0, which is connected to a momentary button.
        When changes occur, an interrupt fires and it sends an appropriate message across the CAN bus.
        If the pin is high, it sends 0xFF. If the pin is low, it sends 0x00.
*/

// Set up external interrupts for INT0 for any logical change
int initButton() {
    EICRA = _BV(ISC00);
    EIMSK = _BV(INT0);
    return(0);
}

// Interrupt routine for External Interrupt 0 (fires when pin 14 changes)
// Reads the value of pin 14 and sends an appropriate CAN message.
ISR(INT0_vect) {
    char cSREG = SREG; //store SREG
    int val = PIND & _BV(PD6);
    if (val) {
        // sendCANMsg(1);
    } else {
        // sendCANMsg(0);
    }
    SREG=cSREG; //restore SREG
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

