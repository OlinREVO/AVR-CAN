#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "api.h"
#include "uart.h"

#define maxDataLength 8

//DemoNode Id
int NODE_HOME = NODE_demoNode3;
int NODE_TARGET_1 = NODE_demoNode1;
//int NODE_TARGET_2 = NODE_demoNode3;


// Set up external interrupts for INT0 for any logical change
int initButton() {
    EICRA = _BV(ISC00) | _BV(ISC30);
    EIMSK = _BV(INT0) | _BV(INT3);

    return(0);
}

void buttonScript(int target, int val, uint8_t x, uint8_t y){
char cSREG = SREG; //Store SREG
    uint8_t msg[1];
    if (val) {
        msg[0] = x; // turn top LED on
    } else {
        msg[0] = y; // turn top LED off
    }
    sendCANmsg(target, MSG_demoMsg, msg, 1);
SREG = cSREG;
}

ISR(INT0_vect) {
    buttonScript(NODE_TARGET_1, PIND & _BV(PD6), 0b01, 0b00);
}

ISR(INT3_vect) {
    buttonScript(NODE_TARGET_1, PINC & _BV(PC0), 0b11, 0b10);
}

// TODO: change this method for each of the demo nodes
void handleCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen) {
    uint8_t cmd = msg[0];
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
}

int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    //DDRB &= ~(_BV(PB2)); // set pin 16 for input
    DDRC &= ~(_BV(PC0)); // set pin 30 for input
    DDRD &= ~(_BV(PD6)); // set pin 14 for input
  
    // Setting PE1 and PE2. XTAL1 to input and XTAL2 to ouput . Pins 10 and 11
    DDRE |= _BV(PE2);
    DDRE &= ~(_BV(PE1));
    
    sei(); // enable global interrupts    
    initCAN(NODE_HOME); // initialize CAN bus
    initButton(); // intitialize button interrupts

    for (;;) {
        // listen for button presses forever
    }
}
