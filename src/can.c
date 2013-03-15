#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <stdlib.h>

int initCan () {
    // enable interrupts: all, receive, transmit
    CANGIE = (_BV(ENIT) | _BV(ENRX));
    CANIE1 = 0; // compatibility with future chips
    CANIE2 = (_BV(IEMOB0) | _BV(IEMOB1)); // enable interrupts on first two MObs
    CANPAGE = 0; // first MOb
    CANCDMOB = _BV(CONMOB1); // enable reception
    CANPAGE = _BV(MOBNB0); // first MOb
    CANCDMOB = _BV(CONMOB1); // enable reception
    return(0);
}

// handle CAN interrupt
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
        int j;
        for (j = 0; j < 8; ++j) {
            PORTB = data[i][j];
            _delay_loop_2(30000)
        }
    }
    free(data);
    SREG=cSREG; //restore SREG
    return(0);
}

int sendCANMsg() {
    uint8_t data = 0b00011011;
    uint8_t dataLength = 1;
    CANCDMOB &= ~(_BV(DLC3) | _BV(DLC2) | _BV(DLC1) | _BV(DLC0)) 
    CANCDMOB |= dataLength;

    return(0);
}

int main () {
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

