#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>
#include "utils/DELAY.h"
#include "utils/SPI.h"

#define A0_PORT PORTL
#define A0_PIN  PL0
#define RST_PORT PORTL
#define RST_PIN  PL1

constexpr uint8_t ST7735_WIDTH  = 128;
constexpr uint8_t ST7735_HEIGHT = 128;

constexpr uint8_t X_OFFSET = 2;
constexpr uint8_t Y_OFFSET = 1;

void HardwareReset();
void ST7735_init();
void ST7735_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void ST7735_drawPixel(uint8_t x, uint8_t y, uint16_t color);

#endif