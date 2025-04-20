#include "utils/ADC.h"

void ADC_INIT() {
    ADMUX = (1 << REFS0); 
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

unsigned int ADC_READ(unsigned char chnl) {
    chnl &= 0x07;

    ADMUX = (ADMUX & 0xF8) | chnl;
    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC));

    uint8_t low = ADCL;
    uint8_t high = ADCH;

    return (high << 8) | low;
}