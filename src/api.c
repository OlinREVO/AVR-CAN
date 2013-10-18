#include "api.h"

// must be called before CAN can be used
// Sample call: initCAN(NODE_speedometer);
int initCAN(int nodeID) {
    
}

// Sample call: sendCANmsg(NODE_watchdog,MSG_critical,data,dataLen);
int sendCANmsg(uint8_t destID, uint8_t msgID, char* msg, uint8_t msgLength) {
    // set MOb number (0 for testing) and auto-increment bits in CAN page MOb register
    CANPAGE = ( _BV(AINC));

    //Wait for MOb1 to be free
    // TODO: This is not good practice; take another look later
    while(CANEN2 & (1 << ENMOB0));
    CANEN2 |= (1 << ENMOB0); //Claim MOb1

    //Clear MOb status register
    CANSTMOB = 0x00;

    int i;
    for (i = 0; i < msgLength; ++i) {
        // while data remains, write it into the data page register
        CANMSG = msg[i];
    }

    // set compatibility registers, RTR bit, and reserved bit to 0
    CANIDT4 = 0;
    CANIDT3 = 0;

    // set ID tag registers
    uint8_t idtag = ((destID & 0x1F) << 6) | (msgID & 0x3F);
    CANIDT2 = ((idtag & 0x07) << 5); // bits 0-2 of idtag (0b0111)
    CANIDT1 = ((idtag & 0x7F8)); // bits 3-10 of idtag (0b11111111000)

    CANCDMOB = (_BV(CONMOB0) | msgLength); // set transmit bit and data length bits of MOb control register

    //TODO: use interrupts for this instead of while loop
    //wait for TXOK
    while((CANSTMOB & (1 << TXOK)) != (1 << TXOK));// & timeout--);

    //Disable Transmission
    CANCDMOB = 0x00;
    //Clear TXOK flag (and all others)
    CANSTMOB = 0x00;

    return(0);
}

// handles the CAN interrupts depending on what kind of interrupt it is
ISR(CAN_INT_vect) {
    char cSREG = SREG; //store SREG
    if (CANSTMOD & _BV(RXOK)) {
        CANSTMOB &= ~(_BV(RXOK)); // reset receive interrupt flag
        readMsg();
    } else if (CANSTMOD & _BV(TXOK)) {
        CANSTMOB &= ~(_BV(RXOK)); // reset transmit interrupt flag
    } else {
        CANSTMOB &= 0; // unknown interrupt
    }
    
    SREG=cSREG; //restore SREG
}

// reads the data and other parameters from a receiving MOb and passes this
//  data over to the implementation of handleCANmsg.
void readMsg(void) {
    CANPAGE &= ~(_BV(INDX2) & _BV(INDX1) & _BV(INDX0)); // set data page 0
    uint8_t msgLength = (CANCDMOB & 0x0F); // last 4 bits are the DLC (0b1111)
    char* receivedMsg;

    // read the data into a local memory block
    receivedMsg = (char*)malloc(sizeof(char)*msgLength);
    int i;
    for (i = 0; i < msgLength; ++i) {
        //while data remains, read it
        receivedMsg[i] = CANMSG;
    }

    // take all of IDT1 and the first 3 bits of IDT2
    uint16_t idtag = (_BV(CANIDT1) << 3) | ((_BV(CANIDT2) & 0xE0) >> 5);
    uint8_t nodeID = (idtag & 0x07C0); // nodeID is bits 6-10 (0b11111000000)
    uint8_t msgID = (idtag & 0x003F); // msgID is bits 0-5 (0b111111)

    // externally-defined handler method
    handleCANmsg(nodeID,msgID,receivedMsg,msgLength);

    free(receivedMsg); // don't want any memory leaks

    CANCDMOB |= _BV(CONMOB1); // set up MOb for reception again
}

/* must be implemented by user - this is just a sample implementation
 * make sure not to do any heavy computation in this method, since it
 *  will be called from an ISR and will delay your main loop

void handleCANmsg(uint8_t destID, uint8_t msgID, char* msg, uint8_t msgLen) {
    
}
*/