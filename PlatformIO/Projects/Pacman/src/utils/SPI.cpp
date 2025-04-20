#include "utils/SPI.h"

void SPI_INIT() {
    DDRB |= (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS);
    SPCR |= (1 << SPE) | (1 << MSTR);
}

void SPI_SEND(char data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}