#ifndef PERIPH_H
#define PERIPH_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

////////// ADC UTILITY FUNCTIONS ///////////

void ADC_init() {
    ADMUX = (1 << REFS0);  // AVcc as reference, ADC0 as input initially
    ADCSRA = (1 << ADEN) |             // ADC Enable
             (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler = 128
}

unsigned int ADC_read(unsigned char chnl) {
    // Sanitize channel input to 0-7
    chnl &= 0x07;

    ADMUX = (ADMUX & 0xF8) | chnl;     // Select ADC channel without affecting other bits
    ADCSRA |= (1 << ADSC);            // Start conversion

    while (ADCSRA & (1 << ADSC));     // Wait for conversion to finish

    uint8_t low = ADCL;
    uint8_t high = ADCH;

    return (high << 8) | low;
}

#endif /* PERIPH_H */
