#include "uart.h"

// define pointers for the circular buffer
// This works by defining a buffer and writing characters to be transmitted
//  into it via the write pointer. The characters are then transmitted them
//  one at a time via the read pointer. If the write pointer passes the end
//  pointer, it loops around to the begin pointer again. If the write pointer
//  meets the read pointer, it returns with an error.
char* TX_begin;
char* TX_end;
char* TX_read;
char* TX_write;

int transmitting;


void initUART(void) {
    // set up the transmit buffer
    TX_begin = (char*)malloc(MAX_TX_BUFFER_SIZE*sizeof(char));
    TX_end = TX_begin + MAX_TX_BUFFER_SIZE*sizeof(char);
    TX_read = TX_begin;
    TX_write = TX_begin;

    transmitting = 0;

    // initialize pins and interrupts
    sei(); // enable global interrupts
    DDRD &= ~(_BV(PD4)); // set pin 12 for input
    DDRD |= _BV(PD3); // set pin 2 for output

    // reset the controller and activate UART
    LINCR = _BV(SWRES) | _BV(LENA) | _BV(LCMD2) | _BV(LCMD0);
    LINBTR = 32;
    LINBRR = 12; // baud rate prescaler
    LINENIR |= _BV(LENTXOK);
}

// turn off the UART controller and free the buffer memory
void endUART(void) {
    transmitting = 0;
    LINCR = _BV(SWRES);
    free(TX_begin);
}

// transmission has completed successfully
// load in next byte if one exists
ISR(LIN_TC_vect) {
    if (TX_read != TX_write) {
        LINDAT = *TX_read;
        TX_read++;
        if (TX_read == TX_end) {
            TX_read = TX_begin;
        }
    } else {
        transmitting = 0;
        LINSIR |= _BV(LTXOK);
    }
}

int UART_transmit() {
    if (transmitting != 1) {
        if (TX_read != TX_write) {
            transmitting = 1;
            LINDAT = *TX_read;
            TX_read++;
            if (TX_read == TX_end) {
                TX_read = TX_begin;
            }
        }
    }
    return(0);
}

int UART_putString(char s[]) {
    int len = strlen(s);
    int i;
    for (i=0;i<len;i++) {
        // add *(s+i) to the circular buffer
        *TX_write = s[i];

        TX_write++;
        if (TX_write == TX_end) {
            TX_write = TX_begin;
        }
    }
    UART_transmit();
    return(0);
}