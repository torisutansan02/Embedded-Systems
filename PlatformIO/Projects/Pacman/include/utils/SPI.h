#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define PIN_SCK   PB1
#define PIN_MOSI  PB2
#define PIN_SS    PB0

void SPI_INIT();
void SPI_SEND(char data);

#endif
