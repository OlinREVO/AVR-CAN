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
    EICRA = _BV(ISC00) | _BV(ISC10) | _BV(ISC30);
    EIMSK = _BV(INT0) | _BV(INT1) | _BV(INT3);

    return(0);
}

// Interrupt routine for External Interrupt 0 (fires when pin 14 changes)
// Reads the value of pin 14 and sends an appropriate CAN message.
// Turns on demo2 LED
ISR(INT0_vect) {
    char cSREG = SREG; //store SREG
    int val = PIND & _BV(PD6);
    uint8_t* msg = (uint8_t*)malloc(1*sizeof(uint8_t));
    if (val) {
        *msg = 0b11; // turn lower LED on
    } else {
        *msg = 0b10; // turn lower LED off
    }
    sendCANmsg(NODE_demoNode1, MSG_demoMsg, msg, 1);
    free(msg);
    // int btnState = (PIND & _BV(PD6)); // down: 0b01000000 / up: 0b00000000
    // if (btnState) {
    //     PORTB |= _BV(PB6); // 0b01000000
    // } else {
    //     PORTB &= ~(_BV(PB6)); // 0b10111111
    // }
    SREG=cSREG; //restore SREG
}

// Interrupt routine for External Interrupt 1 (fires when pin 16 changes)
// Reads the value of pin 16 and sends an appropriate CAN message.
// Turns on broadcast LED
ISR(INT1_vect) {
    char cSREG = SREG; //store SREG
    int val = PIND & _BV(PB2);

    uint8_t* msg = (uint8_t*)malloc(1*sizeof(uint8_t));
    if (val) {
        *msg = 0b11; // turns lower LED on
    } else {
        *msg = 0b10; // turns lower LED off
    }
    sendCANmsg(NODE_demoNode1, MSG_demoMsg, msg, 1);
    if (val) {
        *msg = 0b01; // turns upper LED on
    } else {
        *msg = 0b00; // turns upper LED off
    }
    sendCANmsg(NODE_demoNode2, MSG_demoMsg, msg, 1);
    free(msg);
    // int btnState = (PINB & _BV(PB2));
    // if (btnState) {
    //     PORTB |= _BV(PB4);
    // } else {
    //     PORTB &= ~(_BV(PB4));
    // }
    SREG=cSREG; //restore SREG
}

// Interrupt routine for External Interrupt 3 (fires when pin 30 changes)
// Reads the value of pin 30 and sends an appropriate CAN message.
// Turns on demo3 LED
ISR(INT3_vect) {
    char cSREG = SREG; //store SREG
    int val = PIND & _BV(PC0);
    uint8_t* msg = (uint8_t*)malloc(1*sizeof(uint8_t));
    if (val) {
        *msg = 0b01; // turn upper LED on
    } else {
        *msg = 0b00; // turn upper LED off
    }
    sendCANmsg(NODE_demoNode2, MSG_demoMsg, msg, 1);
    free(msg);
    // int btnState = (PINC & _BV(PC0));
    // if (btnState) {
    //     PORTB |= _BV(PB3);
    // } else {
    //     PORTB &= ~(_BV(PB3));
    // }
    SREG=cSREG; //restore SREG
}

// TODO: change this method for each of the demo nodes
void handleCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen) {
    uint8_t cmd = *msg;
    int ledOn = cmd & 0b01;
    int whichLED = cmd & 0b10;
    if (whichLED) {
        if (ledOn) {
            PORTB |= _BV(PB4);
        } else {
            PORTB &= ~(_BV(PB4));
        }
    } else {
        if (ledOn) {
            PORTB |= _BV(PB6);
        } else {
            PORTB &= ~(_BV(PB6));
        }
    }
    PORTB|=_BV(PB3);
}

int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    DDRB &= ~(_BV(PB2)); // set pin 16 for input
    DDRC &= ~(_BV(PC0)); // set pin 30 for input
    DDRD &= ~(_BV(PD6)); // set pin 14 for input

    sei(); // enable global interrupts    
    initCAN(NODE_demoNode3); // initialize CAN bus
    initButton(); // intitialize button interrupts

    for (;;) {
        // listen for button presses forever
    }
}

