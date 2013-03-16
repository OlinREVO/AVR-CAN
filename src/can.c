#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>

/* TEST CAN CODE
    For this test, we plan to use MOb0 as a receiver and MOb1
    as a transmitter. 
*/


int initCan () {
    // enable interrupts: all, receive
    CANGIE = (_BV(ENIT) | _BV(ENRX));
    CANIE1 = 0; // compatibility with future chips
    CANIE2 = (_BV(IEMOB0) | _BV(IEMOB1)); // enable interrupts on first two MObs

    // set up the first MOb for reception
    CANPAGE = 0; // first MOb
    // set compatibility registers, RTR bit, and reserved bit to 0
    CANIDT4 = 0;
    CANIDT3 = 0;
    // set desired ID tag to receive
    CANIDT2 = 0b10100000; // last 5 bits must be 0
    CANIDT1 = 0b11001100;
    // set compatibility registers to 0, RTR/IDE-mask to 1
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK));
    CANIDM3 = 0;
    // only accept one specific message ID
    CANIDM2 = (_BV(IDMSK2) | _BV(IDMSK1) | _BV(IDMSK0));
    CANIDM1 = 1;
    // enable reception
    CANCDMOB = _BV(CONMOB1); 

    // TODO: set ID tags and masks for msgs this MOb can accept
    return(0);
}

// handle CAN interrupt
// for testing purposes, assuming this is an RX interrupt
int ISR(CANIT_vect) {
    char cSREG = SREG; //store SREG
    CANSTMOB |= _BV(RXOK); // reset receive interrupt flag

    uint8_t bitmask = ~(_BV(INDX2) & _BV(INDX1) & _BV(INDX0)); // data page 0
    CANPAGE &= bitmask; // set data page 0 
    uint8_t dataLength = (CANCDMOB & 00001111); // last 4 bits are the DLC
    // allocate enough space for the data block
    uint8_t* data = (uint8_t *)malloc(dataLength*sizeof(uint8_t));
    int i;
    for (i = 0; i < dataLength; ++i) {
        //while data remains, read it
        data[i] = CANMSG;

        // display received data on LEDs
        PORTB = data[i];
        _delay_loop_2(30000);
    }
    free(data);
    SREG=cSREG; //restore SREG
    return(0);
}

int sendCANMsg() {
    uint8_t data = 0b00011011; // test msg
    uint8_t dataLength = 1; // only sending one test byte
    CANPAGE = (_BV(MOBNB0) | _BV(AINC)); // set MOb number (1 for testing) and auto-increment bits in CAN page MOb register
    CANMSG = data; // set data page register

    // set compatibility registers, RTR bit, and reserved bit to 0
    CANIDT4 = 0;
    CANIDT3 = 0;

    // set ID tag registers
    CANIDT2 = 0b10100000; // last 5 bits must be 0
    CANIDT1 = 0b11001100;

    CANCDMOB = (_BV(CONMOB0) | dataLength); // set transmit bit and data length bits of MOb control register
    return(0);
}

int main (void) {
    unsigned int i;
    // set all PORTB pins for output
    DDRB = 0xFF;

    for (;;) {
        /* toggle PORTB.2 pins */
        PORTB ^= 0xFF;

        for (i = 0; i < 50; i++) {
            _delay_loop_2(30000);
        }
    }

    //return 0;
}

