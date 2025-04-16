/*
Your Name & E-mail: Tristan Cai (tcai019@ucr.edu)
Discussion Section: 024
Assignment: Final Custom Project
I acknowledge all content contained herein, excluding template or example code, is my own original work.
Demo Link: https://www.youtube.com/watch?v=yWW2fQjODDg
*/

#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "spiAVR.h"
#include <time.h>
#include <avr/pgmspace.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

int melody[] = {
    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16,
    NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
    NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,

    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_B5, 32,
    NOTE_FS5, -16, NOTE_DS5, 8, NOTE_DS5, 32, NOTE_E5, 32, NOTE_F5, 32,
    NOTE_F5, 32, NOTE_FS5, 32, NOTE_G5, 32, NOTE_G5, 32, NOTE_GS5, 32, NOTE_A5, 16, NOTE_B5, 8
};

int pacmanPelletsMelody[] = {
    NOTE_G5, 50, NOTE_C6, 50, NOTE_E6, 50, NOTE_G6, 50
};

int pacmanLosingMelody[] = {
    NOTE_AS3, 16,    // A#3 for 4 beats (whole note)
    NOTE_F3, 16,     // F3 for 4 beats (whole note)
    NOTE_DS3, 16,    // D#3 for 4 beats (whole note)
    NOTE_D3, 32,     // D3 for 2 beats (half note)
    
    NOTE_AS3, 16,    // A#3 for 4 beats (whole note)
    NOTE_F3, 16,     // F3 for 4 beats (whole note)
    NOTE_DS3, 16,    // D#3 for 4 beats (whole note)
    NOTE_D3, 32,     // D3 for 2 beats (half note)
    
    NOTE_AS3, 16,    // A#3 for 4 beats (whole note)
    NOTE_F3, 16,     // F3 for 4 beats (whole note)
    NOTE_DS3, 16,    // D#3 for 4 beats (whole note)
    NOTE_D3, 32,     // D3 for 2 beats (half note)
    
    NOTE_G3, 16,     // G3 for 4 beats (whole note)
    NOTE_E3, 16,     // E3 for 4 beats (whole note)
    NOTE_C3, 64     // C3 for 1 beat (quarter note)
};

#define ST7735_WIDTH  128
#define ST7735_HEIGHT 128

void HardwareReset() {
    PORTB = SetBit(PORTB, 0, 0);
    _delay_ms(200);
    PORTB = SetBit(PORTB, 0, 1);
    _delay_ms(200);
}

void ST7735_init() {
    HardwareReset();
    PORTB = SetBit(PORTB, 1, 0); // Send Command
    SPI_SEND(0x01);
    _delay_ms(150);
    SPI_SEND(0x11);
    _delay_ms(200);
    SPI_SEND(0x3A);

    PORTB = SetBit(PORTB, 1, 1); // Send Data
    SPI_SEND(0x05);
    _delay_ms(10);

    PORTB = SetBit(PORTB, 1, 0); // Send Command
    SPI_SEND(0x29);
    _delay_ms(200);
}

#define MAP_WIDTH 32
#define MAP_HEIGHT 32

#define WALL 1
#define PATH 0
#define DOT 2
#define PACMAN 3
#define PINKY 4
#define BLINKY 5
#define CLYDE 6
#define POWER 7

const uint8_t resetMap[MAP_HEIGHT][MAP_WIDTH] PROGMEM = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 1, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1, 6, 0, 0, 0, 0, 0, 2, 0, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 7, 0, 0, 0, 0, 2, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 7, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 0, 2, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 0, 2, 0, 1, 1, 1, 1, 1, 1, 0, 2, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 0, 2, 0, 1},
    {1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const uint8_t menu[MAP_HEIGHT][MAP_WIDTH] PROGMEM = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
};

#define PIXEL_SIZE 4
#define ST7735_BLACK   0x0000
#define ST7735_WHITE   0xFFFF
#define ST7735_YELLOW  0x07FF
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_BLUE    0x001F
#define ST7735_MAGENTA 0xF81F
#define ST7735_ORANGE  0x00BB
#define ST7735_CYAN    0xFFE0

void ST7735_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    PORTB = SetBit(PORTB, 1, 0);
    SPI_SEND(0x2A);

    PORTB = SetBit(PORTB, 1, 1); // Send Data
    SPI_SEND(0x00);
    SPI_SEND(x0);      // X start
    SPI_SEND(0x00);
    SPI_SEND(x1);      // X end

    PORTB = SetBit(PORTB, 1, 0); // Send Command
    SPI_SEND(0x2B); // Row addr set

    PORTB = SetBit(PORTB, 1, 1); // Send Data
    SPI_SEND(0x00);
    SPI_SEND(y0);      // Y start
    SPI_SEND(0x00);
    SPI_SEND(y1);      // Y end

    PORTB = SetBit(PORTB, 1, 0); // Send Command
    SPI_SEND(0x2C); // Write to RAM
}

void ST7735_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;

    ST7735_setAddrWindow(x, y, x, y);

    PORTB = SetBit(PORTB, 1, 1); // Send Data

    SPI_SEND((color >> 8) & 0xFF);
    SPI_SEND(color & 0xFF);
}

void drawBlock(uint8_t x, uint8_t y, uint16_t color) {
    for (uint8_t i = 0; i < PIXEL_SIZE; i++) {
        for (uint8_t j = 0; j < PIXEL_SIZE; j++) {
            ST7735_drawPixel(x * PIXEL_SIZE + i, y * PIXEL_SIZE + j, color);
        }
    }
}

uint8_t pacmanMap[MAP_HEIGHT][MAP_WIDTH];
void drawMap() {
    for (uint8_t y = 0; y < MAP_HEIGHT; y++) {
        for (uint8_t x = 0; x < MAP_WIDTH; x++) {
            if (pacmanMap[y][x] == WALL) {
                drawBlock(x, y, ST7735_RED); // Draw walls in white
            } 
            else if (pacmanMap[y][x] == PATH) {
                drawBlock(x, y, ST7735_BLACK); // Draw paths in black
            } 
            else if (pacmanMap[y][x] == DOT) {
                drawBlock(x, y, ST7735_WHITE); // Draw dots in yellow
            }
            else if (pacmanMap[y][x] == PACMAN) {
                drawBlock(x, y, ST7735_YELLOW);
            }
            else if (pacmanMap[y][x] == PINKY) {
                drawBlock(x, y, ST7735_MAGENTA);
            }
            else if (pacmanMap[y][x] == BLINKY) {
                drawBlock(x, y, ST7735_CYAN);
            }
            else if (pacmanMap[y][x] == CLYDE) {
                drawBlock(x, y, ST7735_GREEN);
            }
            else if (pacmanMap[y][x] == POWER) {
                drawBlock(x, y, ST7735_ORANGE);
            }
        }
    }
}

void custom_delay_us(uint16_t microseconds) {
    for (uint16_t i = 0; i < microseconds; i++) {
        _delay_us(1);
    }
}

void custom_delay_ms(uint16_t milliseconds) {
    for (uint16_t i = 0; i < milliseconds; i++) {
        custom_delay_us(1000);
    }
}

void playNote(int note, int duration) {
    uint16_t cycles = 1000000 / note;
    uint16_t iterations = duration * 1000 / cycles;

    while (iterations--) {
        PORTD |= (1 << PD6);
        custom_delay_us(cycles / 2);
        PORTD &= ~(1 << PD6);
        custom_delay_us(cycles / 2);
    }
}

int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int losingNotes = sizeof(pacmanLosingMelody) / sizeof(pacmanLosingMelody[0]) / 2;
int tempo = 120;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

const unsigned long GCD_PERIOD = 1;

unsigned char gameStart = 0;

unsigned char joystickUp = 0;
unsigned char joystickDown = 0;
unsigned char joystickRight = 0;
unsigned char joystickLeft = 0;
unsigned char joystickBtn = 0;

unsigned char x = 1;
unsigned char y = 30;

unsigned char xp = 11;
unsigned char yp = 9;

unsigned char xb = 11;
unsigned char yb = 10;

unsigned char xc = 11;
unsigned char yc = 11;

unsigned char score = 0;
unsigned char dot = 0;
unsigned char superPac = 0;
unsigned char counter = 0;

#define NUM_TASKS 5

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

task tasks[NUM_TASKS];

enum Melody { IDLE, BUZZER };
int Melody_Tick(int state);

enum Joystick { WAIT, BUTTON, UP, DOWN, LEFT, RIGHT };
int Joystick_Tick(int state);

enum Game { GAME_ON, GAME_OFF };
int Game_Tick(int state);

enum Enemy { ENEMY_OFF, ENEMY_ON };
int Enemy_Tick(int state);

enum Pacman { PACMAN_OFF, PACMAN_ON };
int Pacman_Tick(int state);

void TimerISR() {
    for (unsigned int i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime == tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}

int main(void) {
    DDRB = 0xFF;
    DDRD = 0xFF;
    DDRC = 0x00;
    PORTB = 0x00;
    PORTD = 0x00;
    PORTC = 0xFF;

    ADC_init();
    SPI_INIT();
    ST7735_init();

    int i = 0;

    time_t current_time = time(NULL);
    srand((unsigned int) current_time);

    tasks[i].period = 1;
    tasks[i].state = IDLE;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Melody_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = WAIT;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Joystick_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = GAME_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Game_Tick;
    ++i;
    tasks[i].period = 300;
    tasks[i].state = ENEMY_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Enemy_Tick;
    ++i;
    tasks[i].period = 157;
    tasks[i].state = PACMAN_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Pacman_Tick;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {}

    return 0;
}

int Melody_Tick(int state) {
    int thisNote = 0;
    switch(state) { // State Transitions
        case IDLE:
            if (joystickBtn) {
                state = BUZZER;
            }
            else {
                state = IDLE;
            }
            break;
        case BUZZER:
            state = IDLE;
            break;
        default:
            state = IDLE;
            break;
    }
    switch(state) { // State Actions
        case IDLE:
            break;
        case BUZZER:
            while (thisNote < notes * 2) {
                if (gameStart) {
                    divider = melody[thisNote + 1];
                    if (divider > 0) {
                        noteDuration = (wholenote) / divider;
                    } else if (divider < 0) {
                        noteDuration = (wholenote) / fabs(divider);
                        noteDuration *= 3;
                    }
                    playNote(melody[thisNote], noteDuration);
                }
                uint16_t delayTime = noteDuration * 0.5;
                custom_delay_ms(delayTime);

                thisNote += 2;
            }
            break;
        default:
            break;
    }
    return state;
}

int Joystick_Tick(int state) {
    switch (state) { // State Transitions
        case WAIT:
            if (GetBit(PINC, 2) == 0) {
                state = BUTTON;
            }
            else if (ADC_read(1) < 400) {
                state = UP;
            }
            else if (ADC_read(1) > 600) {
                state = DOWN;
            }
            else if (ADC_read(0) < 400) {
                state = RIGHT;
            }
            else if (ADC_read(0) > 600) {
                state = LEFT;
            }
            else {
                state = WAIT;
            }
            break;
        case BUTTON:
            if (GetBit(PINC, 2) == 1) {
                state = WAIT;
            }
            break;
        case UP:
            state = WAIT;
            break;
        case DOWN:
            state = WAIT;
            break;
        case LEFT:
            state = WAIT;
            break;
        case RIGHT:
            state = WAIT;
            break;
        default:
            state = WAIT;
            break;
    }
    switch (state) {
        case WAIT: // State Actions
            joystickBtn = 0;
            joystickUp = 0;
            joystickDown = 0;
            joystickRight = 0;
            joystickLeft = 0;
            break;
        case BUTTON:
            joystickBtn = 1;
            break;
        case UP:
            joystickUp = 1;
            break;
        case DOWN:
            joystickDown = 1;
            break;
        case LEFT:
            joystickLeft = 1;
            break;
        case RIGHT:
            joystickRight = 1;
            break;
        default:
            break;
    }
    return state;
}

int Game_Tick(int state) {
    int thisNote = 0;
    bool displayMenu = false;
    switch(state) {
        case GAME_OFF:
            dot = 0;
            if (joystickBtn) {
                for (int i = 0; i < MAP_HEIGHT; i++) {
                    for (int j = 0; j < MAP_WIDTH; j++) {
                        pacmanMap[i][j] = pgm_read_byte(&(resetMap[i][j]));
                    }
                }
                drawMap();
                gameStart = 1;
                state = GAME_ON;
            }
            else if (!joystickBtn && !displayMenu) {
                for (int i = 0; i < MAP_HEIGHT; i++) {
                    for (int j = 0; j < MAP_WIDTH; j++) {
                        pacmanMap[i][j] = pgm_read_byte(&(menu[i][j]));
                        if ((score == 1) && (j == 8) && (i >= 23) && (i <= 27)) {
                            pacmanMap[i][j] = 0;
                        }
                        else if ((score == 1) && (j == 6 || j == 7) && (i >= 23) && (i <= 27)) {
                            pacmanMap[i][j] = 1;
                        }
                    }
                }
                drawMap();
                displayMenu = true;
                state = GAME_OFF;
            }
            break;
        case GAME_ON:
            if (
            joystickBtn
            || dot == 64
            || ((x == xp && y == yp) && !superPac)
            || ((x == xb && y == yb) && !superPac)
            || ((x == xc && y == yc) && !superPac)
            ) {
                if (dot == 64) {
                    x = 1;
                    y = 30;

                    xp = 11;
                    yp = 9;

                    xb = 11;
                    yb = 10;

                    xc = 11;
                    yc = 11;

                    score++;
                }
                else {
                    x = 1;
                    y = 30;

                    xp = 11;
                    yp = 9;

                    xb = 11;
                    yb = 10;

                    xc = 11;
                    yc = 11;

                    score = 0;
                    while (thisNote < losingNotes * 2) {
                        divider = pacmanLosingMelody[thisNote + 1];
                        noteDuration = (wholenote) / divider;
                        playNote(pacmanLosingMelody[thisNote], noteDuration);
                        uint16_t delayTime = noteDuration * 0.5;
                        custom_delay_ms(delayTime);

                        thisNote += 2;
                    }
                }
                gameStart = 0;
                state = GAME_OFF;
            }
            else {
                state = GAME_ON;
            }
            break;
        default:
            state = GAME_OFF;
            break;
    }
    switch (state) {
        case GAME_OFF:
            break;
        case GAME_ON:
            break;
        default:
            break;
    }
    return state;
}

int Enemy_Tick(int state) {
    unsigned char randNum1 = rand() % 4;
    unsigned char randNum2 = rand() % 4;
    unsigned char randNum3 = rand() % 4;

    switch (state) { // State Transitions
        case ENEMY_OFF:
            if (gameStart) {
                state = ENEMY_ON;
            }
            break;
        case ENEMY_ON:
            if (!gameStart) {
                state = ENEMY_OFF;
            }
            break;
        default:
            state = ENEMY_OFF;
            break;
    }
    switch (state) { // State Actions
        case ENEMY_OFF:
            break;
        case ENEMY_ON:
            randNum1 = rand() % 4;
            randNum2 = rand() % 4;
            randNum3 = rand() % 4;

            if ((pacmanMap[yp - 1][xp] != WALL)
            && (pacmanMap[yp - 1][xp] != PINKY)
            && (pacmanMap[yp - 1][xp] != BLINKY)
            && (pacmanMap[yp - 1][xp] != CLYDE)
            && randNum1 == 0) {
                if (pacmanMap[yp][xp] == DOT) {
                    drawBlock(xp, yp, ST7735_WHITE);
                } 
                else {
                    drawBlock(xp, yp, ST7735_BLACK);
                }
                yp--;

                if (superPac) {
                    drawBlock(xp, yp, ST7735_RED);
                }
                else {
                    drawBlock(xp, yp, ST7735_MAGENTA);
                }

                if (superPac && (x == xp && y == yp)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xp, yp, ST7735_BLACK);
                    xp = 11;
                    yp = 9;
                    drawBlock(xp, yp, ST7735_RED);
                }
            }
            else if ((pacmanMap[yp + 1][xp] != WALL) 
            && (pacmanMap[yp + 1][xp] != PINKY)
            && (pacmanMap[yp + 1][xp] != BLINKY)
            && (pacmanMap[yp + 1][xp] != CLYDE)
            && randNum1 == 1) {
                if (pacmanMap[yp][xp] == DOT) {
                    drawBlock(xp, yp, ST7735_WHITE);
                } 
                else {
                    drawBlock(xp, yp, ST7735_BLACK);
                }
                yp++;

                if (superPac) {
                    drawBlock(xp, yp, ST7735_RED);
                }
                else {
                    drawBlock(xp, yp, ST7735_MAGENTA);
                }

                if (superPac && (x == xp && y == yp)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xp, yp, ST7735_BLACK);
                    xp = 11;
                    yp = 9;
                    drawBlock(xp, yp, ST7735_RED);
                }
            }
            else if ((pacmanMap[yp][xp + 1] != WALL)
            && (pacmanMap[yp][xp + 1] != PINKY)
            && (pacmanMap[yp][xp + 1] != BLINKY)
            && (pacmanMap[yp][xp + 1] != CLYDE) 
            && randNum1 == 2) {
                if (pacmanMap[yp][xp] == DOT) {
                    drawBlock(xp, yp, ST7735_WHITE);
                } 
                else {
                    drawBlock(xp, yp, ST7735_BLACK);
                }
                xp++;

                if (xp >= 31) {
                    xp = 1;
                }

                if (superPac) {
                    drawBlock(xp, yp, ST7735_RED);
                }
                else {
                    drawBlock(xp, yp, ST7735_MAGENTA);
                }

                if (superPac && (x == xp && y == yp)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xp, yp, ST7735_BLACK);
                    xp = 11;
                    yp = 9;
                    drawBlock(xp, yp, ST7735_RED);
                }
            }
            else if ((pacmanMap[yp][xp - 1] != WALL) 
            && (pacmanMap[yp][xp - 1] != PINKY)
            && (pacmanMap[yp][xp - 1] != BLINKY)
            && (pacmanMap[yp][xp - 1] != CLYDE) 
            && randNum1 == 3) {
                if (pacmanMap[yp][xp] == DOT) {
                    drawBlock(xp, yp, ST7735_WHITE);
                } 
                else {
                    drawBlock(xp, yp, ST7735_BLACK);
                }
                xp--;

                if (xp <= 0) {
                    xp = 30;
                }

                if (superPac) {
                    drawBlock(xp, yp, ST7735_RED);
                }
                else {
                    drawBlock(xp, yp, ST7735_MAGENTA);
                }

                if (superPac && (x == xp && y == yp)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xp, yp, ST7735_BLACK);
                    xp = 11;
                    yp = 9;
                    drawBlock(xp, yp, ST7735_RED);
                }
            }

            if ((pacmanMap[yb - 1][xb] != WALL) 
            && (pacmanMap[yb - 1][xb] != PINKY)
            && (pacmanMap[yb - 1][xb] != BLINKY)
            && (pacmanMap[yb - 1][xb] != CLYDE) 
            && randNum2 == 0) {
                if (pacmanMap[yb][xb] == DOT) {
                    drawBlock(xb, yb, ST7735_WHITE);
                } 
                else {
                    drawBlock(xb, yb, ST7735_BLACK);
                }
                yb--;

                if (superPac) {
                    drawBlock(xb, yb, ST7735_RED);
                }
                else {
                    drawBlock(xb, yb, ST7735_CYAN);
                }

                if (superPac && (x == xb && y == yb)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xb, yb, ST7735_BLACK);
                    xb = 11;
                    yb = 10;
                    drawBlock(xb, yb, ST7735_RED);
                }
            }
            else if ((pacmanMap[yb + 1][xb] != WALL) 
            && (pacmanMap[yb + 1][xb] != PINKY)
            && (pacmanMap[yb + 1][xb] != BLINKY)
            && (pacmanMap[yb + 1][xb] != CLYDE) 
            && randNum2 == 1) {
                if (pacmanMap[yb][xb] == DOT) {
                    drawBlock(xb, yb, ST7735_WHITE);
                } 
                else {
                    drawBlock(xb, yb, ST7735_BLACK);
                }
                yb++;

                if (superPac) {
                    drawBlock(xb, yb, ST7735_RED);
                }
                else {
                    drawBlock(xb, yb, ST7735_CYAN);
                }

                if (superPac && (x == xb && y == yb)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xb, yb, ST7735_BLACK);
                    xb = 11;
                    yb = 10;
                    drawBlock(xb, yb, ST7735_RED);
                }
            }
            else if ((pacmanMap[yb][xb + 1] != WALL) 
            && (pacmanMap[yb][xb + 1] != PINKY)
            && (pacmanMap[yb][xb + 1] != BLINKY)
            && (pacmanMap[yb][xb + 1] != CLYDE) 
            && randNum2 == 2) {
                if (pacmanMap[yb][xb] == DOT) {
                    drawBlock(xb, yb, ST7735_WHITE);
                } 
                else {
                    drawBlock(xb, yb, ST7735_BLACK);
                }
                xb++;

                if (xb >= 31) {
                    xb = 1;
                }

                if (superPac) {
                    drawBlock(xb, yb, ST7735_RED);
                }
                else {
                    drawBlock(xb, yb, ST7735_CYAN);
                }

                if (superPac && (x == xb && y == yb)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xb, yb, ST7735_BLACK);
                    xb = 11;
                    yb = 10;
                    drawBlock(xb, yb, ST7735_RED);
                }
            }
            else if ((pacmanMap[yb][xb - 1] != WALL) 
            && (pacmanMap[yb][xb - 1] != PINKY)
            && (pacmanMap[yb][xb - 1] != BLINKY)
            && (pacmanMap[yb][xb - 1] != CLYDE) 
            && randNum2 == 3) {
                if (pacmanMap[yb][xb] == DOT) {
                    drawBlock(xb, yb, ST7735_WHITE);
                } 
                else {
                    drawBlock(xb, yb, ST7735_BLACK);
                }
                xb--;

                if (xb <= 0) {
                    xb = 30;
                }

                if (superPac) {
                    drawBlock(xb, yb, ST7735_RED);
                }
                else {
                    drawBlock(xb, yb, ST7735_CYAN);
                }

                if (superPac && (x == xb && y == yb)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xb, yb, ST7735_BLACK);
                    xb = 11;
                    yb = 10;
                    drawBlock(xb, yb, ST7735_RED);
                }
            }

            if ((pacmanMap[yc - 1][xc] != WALL) 
            && (pacmanMap[yc - 1][xc] != PINKY)
            && (pacmanMap[yc - 1][xc] != BLINKY)
            && (pacmanMap[yc - 1][xc] != CLYDE) 
            && randNum3 == 0) {
                if (pacmanMap[yc][xc] == DOT) {
                    drawBlock(xc, yc, ST7735_WHITE);
                } 
                else {
                    drawBlock(xc, yc, ST7735_BLACK);
                }
                yc--;

                if (superPac) {
                    drawBlock(xc, yc, ST7735_RED);
                }
                else {
                    drawBlock(xc, yc, ST7735_GREEN);
                }

                if (superPac && (x == xc && y == yc)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xc, yc, ST7735_BLACK);
                    xc = 11;
                    yc = 11;
                    drawBlock(xc, yc, ST7735_RED);
                }
            }
            else if ((pacmanMap[yc + 1][xc] != WALL) 
            && (pacmanMap[yc + 1][xc] != PINKY)
            && (pacmanMap[yc + 1][xc] != BLINKY)
            && (pacmanMap[yc + 1][xc] != CLYDE) 
            && randNum3 == 1) {
                if (pacmanMap[yc][xc] == DOT) {
                    drawBlock(xc, yc, ST7735_WHITE);
                } 
                else {
                    drawBlock(xc, yc, ST7735_BLACK);
                }
                yc++;

                if (superPac) {
                    drawBlock(xc, yc, ST7735_RED);
                }
                else {
                    drawBlock(xc, yc, ST7735_GREEN);
                }

                if (superPac && (x == xc && y == yc)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xc, yc, ST7735_BLACK);
                    xc = 11;
                    yc = 11;
                    drawBlock(xc, yc, ST7735_RED);
                }
            }
            else if ((pacmanMap[yc][xc + 1] != WALL) 
            && (pacmanMap[yc][xc + 1] != PINKY)
            && (pacmanMap[yc][xc + 1] != BLINKY)
            && (pacmanMap[yc][xc + 1] != CLYDE) 
            && randNum3 == 2) {
                if (pacmanMap[yc][xc] == DOT) {
                    drawBlock(xc, yc, ST7735_WHITE);
                } 
                else {
                    drawBlock(xc, yc, ST7735_BLACK);
                }
                xc++;

                if (xc >= 31) {
                    xc = 1;
                }

                if (superPac) {
                    drawBlock(xc, yc, ST7735_RED);
                }
                else {
                    drawBlock(xc, yc, ST7735_GREEN);
                }

                if (superPac && (x == xc && y == yc)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xc, yc, ST7735_BLACK);
                    xc = 11;
                    yc = 11;
                    drawBlock(xc, yc, ST7735_RED);
                }
            }
            else if ((pacmanMap[yc][xc - 1] != WALL) 
            && (pacmanMap[yc][xc - 1] != PINKY)
            && (pacmanMap[yc][xc - 1] != BLINKY)
            && (pacmanMap[yc][xc - 1] != CLYDE) 
            && randNum3 == 3) {
                if (pacmanMap[yc][xc] == DOT) {
                    drawBlock(xc, yc, ST7735_WHITE);
                } 
                else {
                    drawBlock(xc, yc, ST7735_BLACK);
                }
                xc--;

                if (xc <= 0) {
                    xc = 30;
                }

                if (superPac) {
                    drawBlock(xc, yc, ST7735_RED);
                }
                else {
                    drawBlock(xc, yc, ST7735_GREEN);
                }

                if (superPac && (x == xc && y == yc)) {
                    playNote(pacmanPelletsMelody[1], 200);
                    drawBlock(xc, yc, ST7735_BLACK);
                    xc = 11;
                    yc = 11;
                    drawBlock(xc, yc, ST7735_RED);
                }
            }
            break;
        default:
            break;
    }
    return state;
}

int Pacman_Tick(int state) {
    switch (state) {
        case PACMAN_OFF:
            if (gameStart) {
                state = PACMAN_ON;
            }
            break;
        case PACMAN_ON:
            if (!gameStart) {
                state = PACMAN_OFF;
            }
            break;
        default:
            state = PACMAN_OFF;
            break;
    }
    switch (state) {
        case PACMAN_OFF:
            break;
        case PACMAN_ON:
            if (joystickUp && (pacmanMap[y - 1][x] != WALL)) {
                drawBlock(x, y, ST7735_BLACK);
                y--;
                if (pacmanMap[y][x] == DOT || pacmanMap[y][x] == POWER) {
                    if (pacmanMap[y][x] == POWER) {
                        superPac = 1;
                    }
                    dot++;
                    pacmanMap[y][x] = PATH;
                    playNote(pacmanPelletsMelody[0], 20);
                    playNote(pacmanPelletsMelody[1], 20);
                    playNote(pacmanPelletsMelody[2], 20);
                    playNote(pacmanPelletsMelody[3], 20);
                }
                drawBlock(x, y, ST7735_YELLOW);
            }
            else if (joystickDown && (pacmanMap[y + 1][x] != WALL)) {
                drawBlock(x, y, ST7735_BLACK);
                y++;
                if (pacmanMap[y][x] == DOT || pacmanMap[y][x] == POWER) {
                    dot++;
                    if (pacmanMap[y][x] == POWER) {
                        superPac = 1;
                    }
                    pacmanMap[y][x] = PATH;
                    playNote(pacmanPelletsMelody[0], 20);
                    playNote(pacmanPelletsMelody[1], 20);
                    playNote(pacmanPelletsMelody[2], 20);
                    playNote(pacmanPelletsMelody[3], 20);
                }
                drawBlock(x, y, ST7735_YELLOW);
            }
            else if (joystickRight && (pacmanMap[y][x + 1] != WALL)) {
                drawBlock(x, y, ST7735_BLACK);
                x++;
                if (x >= 31) {
                    x = 1;
                }
                if (pacmanMap[y][x] == DOT || pacmanMap[y][x] == POWER) {
                    if (pacmanMap[y][x] == POWER) {
                        superPac = 1;
                    }
                    dot++;
                    pacmanMap[y][x] = PATH;
                    playNote(pacmanPelletsMelody[0], 20);
                    playNote(pacmanPelletsMelody[1], 20);
                    playNote(pacmanPelletsMelody[2], 20);
                    playNote(pacmanPelletsMelody[3], 20);
                }
                drawBlock(x, y, ST7735_YELLOW);
            }
            else if (joystickLeft && (pacmanMap[y][x - 1] != WALL)) {
                drawBlock(x, y, ST7735_BLACK);
                x--;
                if (x <= 0) {
                    x = 30;
                }
                if (pacmanMap[y][x] == DOT || pacmanMap[y][x] == POWER) {
                    dot++;
                    if (pacmanMap[y][x] == POWER) {
                        superPac = 1;
                    }
                    pacmanMap[y][x] = PATH;
                    playNote(pacmanPelletsMelody[0], 20);
                    playNote(pacmanPelletsMelody[1], 20);
                    playNote(pacmanPelletsMelody[2], 20);
                    playNote(pacmanPelletsMelody[3], 20);
                }
                drawBlock(x, y, ST7735_YELLOW);
            }
            if (superPac && counter < 100) {
                playNote(pacmanPelletsMelody[0], 20);
                ++counter;
            }
            if (counter == 100) {
                superPac = 0;
                counter = 0;
            }
            break;
        default:
            break;
    }
    return state;
}