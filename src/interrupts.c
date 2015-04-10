#define F_CPU (1000000L) //pre-defining CPU speed
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//#include "uart.h"



// Set up external interrupts for INT0 for any logical change
int initButton() {
    EICRA = _BV(ISC00);
    EIMSK = _BV(INT0);
    return(0);
}

/*void buttonScript(int target, int val, uint8_t x, uint8_t y){
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
*/
ISR(INT0_vect) {
    PORTB |= _BV(PB4); //set pin PB4 high  
    //buttonScript(NODE_TARGET_1, PIND & _BV(PD6), 0b01, 0b00); //button presses enabled only on pin PD6
}


int main (void) {
    //Refer back to LED tutorial to see why we do this
    DDRB |= 0xFF; // set all PORTB pins for output


    sei(); // enable global interrupts    
    initButton(); // intitialize button interrupts

    for (;;) {
        // keeps led on forever
    }
}