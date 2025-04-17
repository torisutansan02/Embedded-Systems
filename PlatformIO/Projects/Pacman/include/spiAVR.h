#ifndef SPIAVR_H
#define SPIAVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 * Arduino Mega2560 Hardware SPI Pinout:
 * -------------------------------------
 * MOSI (Master Out Slave In)  -> D51 / PB2
 * MISO (Master In Slave Out)  -> D50 / PB3
 * SCK  (SPI Clock)            -> D52 / PB1
 * SS   (Slave Select)         -> D53 / PB0 (You can use any digital output pin as SS)
 *
 * NOTE: For ST7735 displays, SS is typically the CS (chip select) pin,
 * and A0 (D/C) and RESET must also be controlled manually.
 */

// === SPI Pin Bit Positions on Mega ===
#define PIN_SCK   PORTB1  // D52
#define PIN_MOSI  PORTB2  // D51
#define PIN_SS    PORTB0  // D53 (you may assign another pin in software if needed)

// === SPI Initialization ===
void SPI_INIT() {
    // Set MOSI, SCK, SS as outputs
    DDRB |= (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS);

    // Enable SPI, set as Master
    SPCR |= (1 << SPE) | (1 << MSTR);
}

// === SPI Send Function ===
void SPI_SEND(char data) {
    SPDR = data;                       // Start transmission
    while (!(SPSR & (1 << SPIF)));     // Wait for transmission complete
}

#endif /* SPIAVR_H */
