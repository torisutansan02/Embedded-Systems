#include "Ghost.h"
#include <stdlib.h>
#include <math.h>

extern uint8_t pacmanMap[32][32];
extern uint8_t superPac;
extern uint8_t x, y;

void drawBlock(uint8_t x, uint8_t y, uint16_t color);
void playNote(int note, int duration);

#define ST7735_BLACK   0x0000
#define ST7735_WHITE   0xFFFF
#define ST7735_MAGENTA 0xF81F
#define ST7735_CYAN    0xFFE0
#define ST7735_GREEN   0x07E0
#define ST7735_RED     0xF800

#define WALL 1
#define DOT 2
#define PINKY 4
#define BLINKY 5
#define CLYDE 6
#define POWER 7
#define PATH 0

extern int pacmanPelletsMelody[];

Ghost::Ghost(uint8_t ghostType, uint8_t startX, uint8_t startY)
    : x(startX), y(startY), startX(startX), startY(startY), type(ghostType), underTile(PATH) {}

void Ghost::reset() {
    x = startX;
    y = startY;
}

uint8_t Ghost::getX() const { return x; }
uint8_t Ghost::getY() const { return y; }

void Ghost::update(uint8_t pacX, uint8_t pacY) {
    move(pacX, pacY);
}

void Ghost::move(uint8_t pacX, uint8_t pacY) {
    const int8_t dir[4][2] = { {0, -1}, {0, 1}, {1, 0}, {-1, 0} };
    bool visited[32][32] = { false };
    uint8_t parentX[32][32], parentY[32][32];

    struct Node {
        uint8_t x, y;
    };

    Node queue[1024];
    int front = 0, back = 0;

    queue[back++] = { x, y };
    visited[y][x] = true;

    bool found = false;

    while (front < back) {
        Node current = queue[front++];

        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dir[i][0];
            int ny = current.y + dir[i][1];

            if (nx < 0) nx = 30;
            if (nx >= 32) nx = 1;
            if (ny < 0 || ny >= 32) continue;

            if (visited[ny][nx]) continue;

            uint8_t cell = pacmanMap[ny][nx];
            if (cell == WALL || cell == PINKY || cell == BLINKY || cell == CLYDE) continue;

            visited[ny][nx] = true;
            parentX[ny][nx] = current.x;
            parentY[ny][nx] = current.y;

            queue[back++] = { (uint8_t)nx, (uint8_t)ny };

            if (nx == pacX && ny == pacY) {
                found = true;
                break;
            }
        }
        if (found) break;
    }

    eraseGhost();

    if (found) {
        uint8_t tx = pacX;
        uint8_t ty = pacY;

        while (!(parentX[ty][tx] == x && parentY[ty][tx] == y)) {
            uint8_t px = parentX[ty][tx];
            uint8_t py = parentY[ty][tx];
            tx = px;
            ty = py;
        }

        if (pacmanMap[ty][tx] != PINKY && pacmanMap[ty][tx] != BLINKY && pacmanMap[ty][tx] != CLYDE) {
            x = tx;
            y = ty;
        }
    } else {
        double minDist = 1e9;
        uint8_t bestX = x, bestY = y;

        for (int i = 0; i < 4; ++i) {
            int nx = x + dir[i][0];
            int ny = y + dir[i][1];

            if (nx < 0) nx = 30;
            if (nx >= 32) nx = 1;
            if (ny < 0 || ny >= 32) continue;

            uint8_t cell = pacmanMap[ny][nx];
            if (cell == WALL || cell == PINKY || cell == BLINKY || cell == CLYDE) continue;

            double dist = (pacX - nx) * (pacX - nx) + (pacY - ny) * (pacY - ny);
            if (dist < minDist) {
                minDist = dist;
                bestX = nx;
                bestY = ny;
            }
        }

        if (bestX != x || bestY != y) {
            x = bestX;
            y = bestY;
        }
    }

    if (superPac && x == pacX && y == pacY) {
        playNote(pacmanPelletsMelody[1], 200);
        drawBlock(x, y, ST7735_BLACK);
        reset();
    }

    drawGhost();
}

void Ghost::eraseGhost() {
    pacmanMap[y][x] = underTile;
    drawBlock(x, y, underTile == DOT || underTile == POWER ? ST7735_WHITE : ST7735_BLACK);
}

void Ghost::drawGhost() {
    underTile = pacmanMap[y][x];

    uint16_t color = superPac ? ST7735_RED :
                     (type == PINKY)  ? ST7735_MAGENTA :
                     (type == BLINKY) ? ST7735_CYAN :
                     ST7735_GREEN;

    pacmanMap[y][x] = type;
    drawBlock(x, y, color);
}

bool Ghost::isAtPosition(uint8_t px, uint8_t py) const {
    return (x == px && y == py);
}
