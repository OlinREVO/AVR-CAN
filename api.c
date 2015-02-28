#include "api.h"

// must be called before CAN can be used
// Sample call: initCAN(NODE_speedometer);
int initCAN(uint8_t nodeID) {
    sei(); // enable global interrupts
    
    CANGCON = _BV(SWRES); //Software reset
    CANTCON = 0x00; //CAN timing prescaler set to 0;

    // Set baud rate to 1000kb (assuming 8Mhz IOclk)
    CANBT1 = 0x08; // change this to 0x08 for baud rate of 25000 for Logic debugging
    CANBT2 = 0x04;
    CANBT3 = 0x13;

    // enable interrupts: all, receive
    CANGIE = (_BV(ENIT) | _BV(ENRX));
    // compatibility with future chips
    CANIE1 = 0;
    // enable interrupts on all MObs
    CANIE2 = (_BV(IEMOB0) | _BV(IEMOB1) | _BV(IEMOB2));

    int8_t mob;
    for (mob=0; mob<6; mob++ ) {
        // Selects Message Object 0-5
        CANPAGE = ( mob << 4 );
        // Disable mob
        CANCDMOB = 0x00;
        // Clear mob status register;
        CANSTMOB = 0x00;
    }

    // set up MOb1 for reception of this node's messages
    CANPAGE = _BV(MOBNB1);

    // MOb ID/IDmsk settings
    // set compatibility registers to 0, RTR/IDE-mask to 1
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); // write to 0x00?
    CANIDT4 = 0x00;
    CANIDM3 = 0x00;
    CANIDT3 = 0x00;
    // accept all message IDs (bits 0-5)
    // accept only this node's node ID (bits 6-10)
    CANIDM2 = 0x00;
    CANIDT2 = 0x00;
    CANIDM1 = 0xF8; // 0b11111000
    CANIDT1 = ((nodeID & 0x1F) << 3); // node ID

    // enable reception, DLC8
    CANCDMOB = _BV(CONMOB1) | (8 << DLC0);

    //Set Up Debug LED
    DDRD |= _BV(PD3);


/*    // set up MOb2 for reception of this broadcast messages
    CANPAGE = _BV(MOBNB2);

    // MOb ID/IDmsk settinxgs
    // set compatibility registers to 0, RTR/IDE-mask to 1
    CANIDM4 = (_BV(RTRMSK) | _BV(IDEMSK)); // write to 0x00?
    CANIDT4 = 0x00;
    CANIDM3 = 0x00;
    CANIDT3 = 0x00;
    // accept all message IDs (bits 0-5)
    // accept only the broadcast node ID (bits 6-10)
    CANIDM2 = 0x00;
    CANIDT2 = 0x00;
    CANIDM1 = 0xF8; // 0b11111000
    CANIDT1 = ((NODE_broadcast & 0x1F) << 3); // broadcast ID

    // enable reception, DLC8
    CANCDMOB = _BV(CONMOB1) | (8 << DLC0);*/

    // Enable mode: CAN channel enters in enable mode after 11 recessive bits
    //  This should always be last in the init method
    CANGCON |= ( 1 << ENASTB );

// TODO: set ID tags and masks for msgs this MOb can accept

    return(0);
}

// reads the data and other parameters from a receiving MOb and passes this
//  data over to the implementation of handleCANmsg.
// should not be called by external methods
void readMsg(void) {
    CANPAGE &= ~(_BV(AINC) | _BV(INDX2) | _BV(INDX1) | _BV(INDX0)); // set data page 0
    uint8_t msgLength = (CANCDMOB & 0x0F); // last 4 bits are the DLC (0b1111)
    uint8_t receivedMsg[msgLength];

    // read the data into a local memory block
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

    CANCDMOB |= _BV(CONMOB1); // set up MOb for reception again
}

// Sample call: sendCANmsg(NODE_watchdog,MSG_critical,data,dataLen);
int sendCANmsg(uint8_t destID, uint8_t msgID, uint8_t msg[], uint8_t msgLength) {
    // use MOb 0 for sending and auto-increment bits in CAN page MOb register
    CANPAGE = ( _BV(AINC));

    //Wait for MOb1 to be free
    // TODO: This is not good practice; take another look later
    while(CANEN2 & (1 << ENMOB0)); // Stuck in infinite loop?
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
    uint16_t idtag = ((destID & 0x1F) << 6) | (msgID & 0x3F);
    CANIDT2 = ((idtag & 0x07) << 5); // bits 0-2 of idtag (0b0111)
    CANIDT1 = ((idtag & 0x7F8) >> 3); // bits 3-10 of idtag (0b11111111000)

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

    uint8_t mobIndex = (CANHPMOB & 0xF0) >> 4; // check which MOb received the interrupt
    CANPAGE &= 0x0F; // clear out the top 4 bits (current MOb)
    CANPAGE |= mobIndex << 4; // set the current MOb

    if (CANSTMOB & _BV(RXOK)) {
        CANSTMOB &= ~(_BV(RXOK)); // reset receive interrupt flag
        readMsg();
    } else if (CANSTMOB & _BV(TXOK)) {
        CANSTMOB &= ~(_BV(TXOK)); // reset transmit interrupt flag
    } else {
        CANSTMOB &= 0; // unknown interrupt
    }
    
    SREG=cSREG; //restore SREG
}

/* must be implemented by user - this is just a sample implementation
 * make sure not to do any heavy computation in this method, since it
 *  will be called from an ISR and will delay your main loop

void handleCANmsg(uint8_t destID, uint8_t msgID, char* msg, uint8_t msgLen) {
    
}
*/