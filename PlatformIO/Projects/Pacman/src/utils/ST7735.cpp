#include "utils/ST7735.h"

void HardwareReset() {
    RST_PORT &= ~(1 << RST_PIN);
    custom_delay_us(200);
    RST_PORT |= (1 << RST_PIN);
    custom_delay_us(200);
}

void ST7735_init() {
    DDRL |= (1 << A0_PIN) | (1 << RST_PIN);

    HardwareReset();

    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x01); custom_delay_us(150);
    SPI_SEND(0x11); custom_delay_us(200);

    SPI_SEND(0x3A); A0_PORT |= (1 << A0_PIN); SPI_SEND(0x05); custom_delay_us(10);
    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x29);
    SPI_SEND(0x36); A0_PORT |= (1 << A0_PIN); SPI_SEND(0x00); custom_delay_us(200);
}

void ST7735_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    x0 += X_OFFSET;
    x1 += X_OFFSET;
    y0 += Y_OFFSET;
    y1 += Y_OFFSET;

    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x2A);
    A0_PORT |= (1 << A0_PIN);
    SPI_SEND(0x00); SPI_SEND(x0);
    SPI_SEND(0x00); SPI_SEND(x1);

    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x2B);
    A0_PORT |= (1 << A0_PIN);
    SPI_SEND(0x00); SPI_SEND(y0);
    SPI_SEND(0x00); SPI_SEND(y1);

    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x2C);
}

void ST7735_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;

    ST7735_setAddrWindow(x, y, x, y);
    A0_PORT |= (1 << A0_PIN);
    SPI_SEND((color >> 8) & 0xFF);
    SPI_SEND(color & 0xFF);
}