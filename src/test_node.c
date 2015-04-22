#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

#include "api.h"
#include "uart.h"

#define maxDataLength 8


void handleCANmsg(uint8_t destID, uint8_t msgID, uint8_t* msg, uint8_t msgLen) {
    }


int main (void) {
    DDRB |= 0xFF; // set all PORTB pins for output
    //DDRB &= ~(_BV(PB2)); // set pin 16 for input
    DDRC &= ~(_BV(PC0)); // set pin 30 for input
    DDRD &= ~(_BV(PD6)); // set pin 14 for input
    DDRB &= ~(_BV(PB5)); // pin 26 (adc 6 for input)

    // Setting PE1 and PE2. XTAL1 to input and XTAL2 to ouput . Pins 10 and 11
    DDRE |= _BV(PE2);
    DDRE &= ~(_BV(PE1));

    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    //Enable internal reference voltage
    ADCSRB &= _BV(AREFEN);
    //Set internal reference voltage as AVcc
    ADMUX |= _BV(REFS0);

    ADMUX &= ~(0x1F);
    ADMUX |= 0x06; //(26)

    sei(); // enable global interrupts
    initCAN(NODE_bms); // initialize CAN bus

    uint8_t data[3];
    uint8_t data2[3];
    //uint8_t *data3 = malloc(2 * sizeof(uint8_t));

    for (;;) {
        ADCSRA |=  _BV(ADSC);
        while(bit_is_set(ADCSRA, ADSC));

        //ADC is a macro to combine ADCL and ADCH
        uint16_t pos_reading = ADC;
        data[0] = pos_reading >> 2;
        data[1] = 0x00;
        data[2] = 0x0F;

        data2[0] = 0xFF;
        data2[1] = 0x0F;
        data2[2] = 0x0A;

        //data3[2] = 0x00;

        sendCANmsg(NODE_charger, MSG_speed, data, 3);
        _delay_ms(1000);
//        _delay_ms(1000);

        sendCANmsg(NODE_halleffect, MSG_data_other, data2, 3);
        _delay_ms(1000);
    }
}
