#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "spiAVR.h"
#include "Ghost.h"
#include "Notes.h"
#include "Melody.h"
#include "Map.h"
#include <time.h>
#include <avr/io.h>
#include <util/delay.h>

#define ST7735_WIDTH 128
#define ST7735_HEIGHT 128
#define PIXEL_SIZE 4

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

#define ST7735_BLACK   0x0000
#define ST7735_WHITE   0xFFFF
#define ST7735_YELLOW  0x07FF
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_MAGENTA 0xF81F
#define ST7735_ORANGE  0x00BB
#define ST7735_CYAN    0xFFE0

#define A0_PORT PORTL
#define A0_PIN  PL0       // D49
#define RST_PORT PORTL
#define RST_PIN  PL1       // D48

#define GCD_PERIOD 1
#define NUM_TASKS 5

#define PACMAN_START_X 14
#define PACMAN_START_Y 29
#define MAX_DOTS 62
#define SUPER_PAC_DURATION 100

// === Custom Delay Functions (Fixes _delay_us() issue) ===
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

// ===== Display Setup =====
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
    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x2A);
    A0_PORT |= (1 << A0_PIN);
    SPI_SEND(0x00); SPI_SEND(x0); SPI_SEND(0x00); SPI_SEND(x1);

    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x2B);
    A0_PORT |= (1 << A0_PIN);
    SPI_SEND(0x00); SPI_SEND(y0); SPI_SEND(0x00); SPI_SEND(y1);

    A0_PORT &= ~(1 << A0_PIN); SPI_SEND(0x2C);
}

void ST7735_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT)) return;

    ST7735_setAddrWindow(x, y, x, y);
    A0_PORT |= (1 << A0_PIN);
    SPI_SEND((color >> 8) & 0xFF);
    SPI_SEND(color & 0xFF);
}

// ========== Game State ==========
uint8_t pacmanMap[MAP_HEIGHT][MAP_WIDTH];
uint8_t x = PACMAN_START_X, y = PACMAN_START_Y;
uint8_t joystickUp, joystickDown, joystickLeft, joystickRight, joystickBtn;
uint8_t dot = 0, score = 0, counter = 0, superPac = 0;
bool gameStart = false, justStarted = false, justDied = false, losingPlayed = false;
bool displayMenu = false, menuDrawn = false; // ← ADDED

Ghost pinky(PINKY, 11, 9), blinky(BLINKY, 11, 10), clyde(CLYDE, 11, 11);

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

task tasks[NUM_TASKS];

// ========== Drawing ==========
void drawBlock(uint8_t x, uint8_t y, uint16_t color) {
    if (x >= ST7735_WIDTH / PIXEL_SIZE || y >= ST7735_HEIGHT / PIXEL_SIZE) return;

    for (uint8_t i = 0; i < PIXEL_SIZE; i++) {
        for (uint8_t j = 0; j < PIXEL_SIZE; j++) {
            uint8_t px = x * PIXEL_SIZE + i;
            uint8_t py = y * PIXEL_SIZE + j;

            if (px < ST7735_WIDTH && py < ST7735_HEIGHT) {
                ST7735_drawPixel(px, py, color);
            }
        }
    }
}

void drawMap() {
    for (uint8_t y = 0; y < MAP_HEIGHT; y++) {
        for (uint8_t x = 0; x < MAP_WIDTH; x++) {
            uint16_t color;
            switch (pacmanMap[y][x]) {
                case WALL: color = ST7735_RED; break;
                case PATH: color = ST7735_BLACK; break;
                case DOT: color = ST7735_WHITE; break;
                case PACMAN: color = ST7735_YELLOW; break;
                case PINKY: color = ST7735_MAGENTA; break;
                case BLINKY: color = ST7735_CYAN; break;
                case CLYDE: color = ST7735_GREEN; break;
                case POWER: color = ST7735_ORANGE; break;
                default: color = ST7735_BLACK; break;
            }
            drawBlock(x, y, color);
        }
    }
}

// ========== Sound ==========
void playNote(int note, int duration) {
    uint16_t cycles = 1000000 / note;
    uint16_t iterations = duration * 1000 / cycles;

    while (iterations--) {
        PORTH |= (1 << PH3);
        custom_delay_us(cycles / 2);
        PORTH &= ~(1 << PH3);
        custom_delay_us(cycles / 2);
    }
}

// ========== Game Logic Helpers ==========
void movePacmanTo(uint8_t newX, uint8_t newY) {
    if (pacmanMap[newY][newX] == WALL) return;

    drawBlock(x, y, ST7735_BLACK);
    x = newX; y = newY;

    if (x >= 31) x = 1;
    if (x <= 0) x = 30;

    if (pacmanMap[y][x] == DOT || pacmanMap[y][x] == POWER) {
        if (pacmanMap[y][x] == POWER) superPac = 1;
        pacmanMap[y][x] = PATH;
        dot++;
        for (int i = 0; i < 4; i++) playNote(pacmanPelletsMelody[i], 20);
    }

    drawBlock(x, y, ST7735_YELLOW);
}

// ========== Tick Functions ==========
int Joystick_Tick(int state) {
    joystickBtn = !GetBit(PINF, 2);
    joystickUp = ADC_read(0) > 600;
    joystickDown = ADC_read(0) < 400;
    joystickRight = ADC_read(1) > 600;
    joystickLeft = ADC_read(1) < 400;
    return 0;
}

int Game_Tick(int state) {
    if (!gameStart) {
        if (joystickBtn) {
            displayMenu = false;
            menuDrawn = false; // ← Reset flag when game starts
            dot = score = counter = superPac = 0;
            justStarted = true;
            for (int i = 0; i < MAP_HEIGHT; i++)
                for (int j = 0; j < MAP_WIDTH; j++)
                    pacmanMap[i][j] = pgm_read_byte(&(resetMap[i][j]));
            x = PACMAN_START_X; 
            y = PACMAN_START_Y;
        } 
        
        // ✅ Only draw menu ONCE
        else if (!displayMenu && !menuDrawn) {
            for (int i = 0; i < MAP_HEIGHT; i++)
                for (int j = 0; j < MAP_WIDTH; j++)
                    pacmanMap[i][j] = pgm_read_byte(&(menu[i][j]));
            #define DOT 2
            drawMap();
            displayMenu = true;
            menuDrawn = true; // Prevent further redraws
        }

        return 0;
    }

    // === Game Over Conditions ===
    if ((dot >= MAX_DOTS) || 
        (pinky.isAtPosition(x, y) && !superPac) || 
        (blinky.isAtPosition(x, y) && !superPac) || 
        (clyde.isAtPosition(x, y) && !superPac)) {

        x = PACMAN_START_X;
        y = PACMAN_START_Y;

        pinky.reset(); blinky.reset(); clyde.reset();

        if (dot >= MAX_DOTS) score++;
        else if (!justDied && !losingPlayed) {
            justDied = losingPlayed = true;
            score = 0;
            for (int i = 0; i < losingNotes * 2; i += 2) {
                int d = (wholenote) / pacmanLosingMelody[i + 1];
                playNote(pacmanLosingMelody[i], d);
                custom_delay_ms(d / 2);
            }
            justDied = losingPlayed = false;
        }

        gameStart = false;
        return 0;
    }

    return 1;
}

int Pacman_Tick(int state) {
    if (!gameStart) return 0;

    if (joystickUp)    movePacmanTo(x, y - 1);
    if (joystickDown)  movePacmanTo(x, y + 1);
    if (joystickRight) movePacmanTo(x + 1, y);
    if (joystickLeft)  movePacmanTo(x - 1, y);

    if (superPac && counter++ >= SUPER_PAC_DURATION) {
        superPac = counter = 0;
    }

    return 1;
}

int Enemy_Tick(int state) {
    if (gameStart) {
        pinky.update(x, y);
        blinky.update(x, y);
        clyde.update(x, y);
    }
    return gameStart;
}

int Melody_Tick(int state) {
    static bool playedIntro = false;
    if (joystickBtn && !playedIntro && justStarted) {
        playedIntro = true;
        for (int i = 0; i < notes * 2; i += 2) {
            int d = (wholenote) / abs(melody[i + 1]);
            if (melody[i + 1] < 0) d *= 3;
            playNote(melody[i], d);
            custom_delay_ms(d / 2);
        }
        justStarted = false;
    }
    return 0;
}

// ========== Main ==========
void TimerISR() {
    for (uint8_t i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime >= tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}

int main(void) {
    DDRB = DDRD = DDRH = DDRE = 0xFF;
    DDRF = 0x00;
    PORTB = PORTD = PORTH = PORTE = 0x00;
    PORTF = 0xFF;

    ADC_init(); SPI_INIT(); ST7735_init();

    tasks[0] = { 0, 1, 0, &Joystick_Tick };
    tasks[1] = { 0, 1, 0, &Game_Tick };
    tasks[2] = { 0, 300, 0, &Enemy_Tick };
    tasks[3] = { 0, 150, 0, &Pacman_Tick };
    tasks[4] = { 0, 1, 0, &Melody_Tick };

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1);
}
