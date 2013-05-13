#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define maxDataLength 8

/* TEST CAN CODE - CLIENT
        MOb1 listens continuously for CAN messages. Based on the message content,
        it drives PORTB high (for a message of 0xFF) or low (for 0x00).
        PORTB should be connected to an LED so that it turns on or off when messages are received.
*/

uint8_t receivedData[maxDataLength];
uint8_t data[maxDataLength];


// Set up and enable the CAN bus by initializing the relevant registers
int initCan () {

    //Software reset
    CANGCON = _BV(SWRES);

    //CAN timing prescaler set to 0;
    CANTCON = 0x00;

    // Set baud rate to 1000kb (assuming 8Mhz IOclk)
    CANBT1 = 0x1E;
    CANBT2 = 0x04;
    CANBT3 = 0x13;

    int i = 0;
    for(i=0;i<maxDataLength;i++) {
        receivedData[i] = 0x00;
    }

    CANGIE = (_BV(ENIT) | _BV(ENRX)); // enable interrupts: all, receive
    CANIE1 = 0; // compatibility with future chips
    CANIE2 = (_BV(IEMOB0) | _BV(IEMOB1) | _BV(IEMOB2)); // enable interrupts on all MObs

    int8_t mob;
    for (mob=0; mob<6; mob++ ) {
        CANPAGE = ( mob << 4 ); // Selects Message Object 0-5
        CANCDMOB = 0x00; // Disable mob
        CANSTMOB = 0x00; // Clear mob status register;
    }

    CANPAGE = _BV(MOBNB1); // set up MOb1 for reception


    // set compatibility registers to 0, RTR/IDE-mask to 1
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK));
    CANIDM3 = 0x00;
    /*
    // only accept one specific message ID
    CANIDM2 = (_BV(IDMSK2) | _BV(IDMSK1) | _BV(IDMSK0));
    CANIDM1 = 0x01;*/

    // accept all IDs
    CANIDM2 = 0x00;
    CANIDM1 = 0x00;

    CANCDMOB = _BV(CONMOB1) | (8 << DLC0); // enable reception, DLC8

    // Enable mode: CAN channel enters in enable mode after 11 recessive bits
    CANGCON |= ( 1 << ENASTB );

// TODO: set ID tags and masks for msgs this MOb can accept

    return(0);
}

// handle CAN interrupt
// for testing purposes, assuming this is an RX interrupt
ISR(CAN_INT_vect) {
    char cSREG = SREG; //store SREG
    CANSTMOB &= ~(_BV(RXOK)); // reset receive interrupt flag

    uint8_t bitmask = ~(_BV(INDX2) & _BV(INDX1) & _BV(INDX0)); // data page 0
    CANPAGE &= bitmask; // set data page 0
    uint8_t dataLength = (CANCDMOB & 0x0F); // last 4 bits are the DLC

    // allocate enough space for the data block
    int i;
    for(i=0;i<maxDataLength;i++) {
        receivedData[i] = 0x00;
    }
    for (i = 0; i < maxDataLength; ++i) {
        //while data remains, read it
        receivedData[i] = CANMSG;
    }

    // act on LED on/off message
    if (receivedData[0] == 0xFF) {
        PORTB = 0xFF;
    } else {
        PORTB = 0x00;
    }

    CANCDMOB |= _BV(CONMOB1); // set up MOb for reception again
    SREG=cSREG; //restore SREG
}

int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    sei(); // enable global interrupts
    initCan(); // initialize CAN bus

    for (;;) {
        // listen for CAN messages forever
    }
}

