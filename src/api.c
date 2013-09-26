#include "api.h"

// must be called before CAN can be used
// Sample call: initCAN(NODE_speedometer);
int initCAN(int nodeID) {

}

// implemented in api.c
// Sample call: sendCANmsg(NODE_watchdog,MSG_critical,data,dataLen);
int sendCANmsg(int destID, int msgID, char msg[], int msgLen) {

}

/* must be implemented by user - this is just a sample implementation
 * make sure not to do any heavy computation in this method, since it
 *  will be called from an ISR and will delay your main loop

void handleCANmsg(int destID, int msgID, char msg[], int msgLen) {
    
}
*/