#ifndef ADC_H
#define ADC_H

#include <avr/interrupt.h>

void ADC_INIT();
unsigned int ADC_READ(unsigned char chnl);

#endif
