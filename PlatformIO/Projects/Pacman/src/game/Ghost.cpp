#include "game/Ghost.h"

Ghost::Ghost(
    uint8_t ghostType, 
    uint8_t startX, 
    uint8_t startY
): 
    x(startX), 
    y(startY), 
    startX(startX), 
    startY(startY), 
    type(ghostType), 
    underTile(PATH) 
{}

void Ghost::reset(uint8_t pacX, uint8_t pacY, bool superPac) {
    x = startX;
    y = startY;
    underTile = PATH;
    drawGhost(pacX, pacY, superPac);
}

uint8_t Ghost::getX() const { return x; }
uint8_t Ghost::getY() const { return y; }

void Ghost::update(uint8_t pacX, uint8_t pacY, bool superPac) {
    move(pacX, pacY, superPac);
}

void Ghost::move(uint8_t pacX, uint8_t pacY, bool superPac) {
    const int8_t dir[4][2] = { {0, -1}, {0, 1}, {1, 0}, {-1, 0} };
    bool visited[MAP_HEIGHT][MAP_WIDTH] = { false };
    uint8_t parentX[MAP_HEIGHT][MAP_WIDTH], parentY[MAP_HEIGHT][MAP_WIDTH];

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

            if (nx <= 0) nx = MAP_WIDTH - 1;
            if (nx >= MAP_WIDTH) nx = 1;
            if (ny < 0 || ny >= MAP_HEIGHT) continue;

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

    eraseGhost(pacX, pacY, superPac);

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

            if (nx <= 0) nx = MAP_WIDTH - 1;
            if (nx >= MAP_WIDTH) nx = 1;
            if (ny < 0 || ny >= MAP_HEIGHT) continue;

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
        pacmanMap[y][x] = PATH;
        reset(pacX, pacY, superPac);
        pacmanMap[y][x] = PACMAN;
        drawBlock(x, y, 0xFFE0);
        return;
    }

    drawGhost(pacX, pacY, superPac);
}

void Ghost::eraseGhost(uint8_t pacX, uint8_t pacY, bool superPac) {
    if (x == pacX && y == pacY && !superPac) return;

    pacmanMap[y][x] = underTile;
    drawBlock(x, y, underTile == DOT || underTile == POWER ? ST7735_WHITE : ST7735_BLACK);
}

void Ghost::drawGhost(uint8_t pacX, uint8_t pacY, bool superPac) {
    bool pacmanHere = (x == pacX && y == pacY);
    underTile = pacmanMap[y][x];

    uint16_t color = superPac ? ST7735_DARKBLUE :
                     (type == PINKY)  ? ST7735_MAGENTA :
                     (type == BLINKY) ? ST7735_CYAN :
                     ST7735_GREEN;

    pacmanMap[y][x] = type;

    if (!superPac && pacmanHere) {
        drawBlock(x, y, color);
        return;
    }

    if (!pacmanHere) {
        drawBlock(x, y, color);
    }
}

bool Ghost::isAtPosition(uint8_t px, uint8_t py) const {
    return x == px && y == py;
}
