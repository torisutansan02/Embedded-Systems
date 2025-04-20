#ifndef DISPLAY_H
#define DISPLAY_H

/*
Local Headers
*/
#include "utils/ST7735.h"
#include "game/Constants.h"

/*
C STD Library Headers
AVR Lib C Library Headers
*/
#include <stdint.h>
#include <avr/pgmspace.h>

/*
BGR Format
Hexadecimal: 0xFFFF
Binary: 0b1111111111111111
Decimal: 65535
*/
constexpr uint16_t ST7735_BLACK     = 0x0000;
constexpr uint16_t ST7735_WHITE     = 0xFFFF;
constexpr uint16_t ST7735_RED       = 0x001F;
constexpr uint16_t ST7735_GREEN     = 0x07E0;
constexpr uint16_t ST7735_BLUE      = 0xF800; 
constexpr uint16_t ST7735_YELLOW    = 0x07FF;
constexpr uint16_t ST7735_MAGENTA   = 0xF81F;
constexpr uint16_t ST7735_CYAN      = 0xFFE0;
constexpr uint16_t ST7735_ORANGE    = 0x04FF;
constexpr uint16_t ST7735_DARKBLUE  = 0xFBE0;

constexpr uint8_t PIXEL_SIZE = (128 / MAP_HEIGHT);

extern const uint8_t resetMap[MAP_HEIGHT][MAP_WIDTH] PROGMEM;
extern const uint8_t menu[MAP_HEIGHT][MAP_WIDTH] PROGMEM;
extern uint8_t pacmanMap[MAP_HEIGHT][MAP_WIDTH];

void drawBlock(uint8_t x, uint8_t y, uint16_t color);
void drawMap();
void loadMap(const uint8_t source[MAP_HEIGHT][MAP_WIDTH]);

#endif
