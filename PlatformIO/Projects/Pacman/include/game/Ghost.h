#ifndef GHOST_H
#define GHOST_H

#include "game/Display.h"
#include "game/Sound.h"
#include "game/Constants.h"
#include "game/Pacman.h"
#include <stdint.h>

class Pacman;

class Ghost {
private:
    uint8_t x, y;
    uint8_t startX, startY;
    uint8_t type;
    uint8_t underTile;

public:
    Ghost(uint8_t ghostType, uint8_t startX, uint8_t startY);

    void update(uint8_t pacX, uint8_t pacY, bool superPac);
    void move(uint8_t pacX, uint8_t pacY, bool superPac);
    void reset(uint8_t pacX, uint8_t pacY, bool superPac);
    void drawGhost(uint8_t pacX, uint8_t pacY, bool superPac);
    void eraseGhost(uint8_t pacX, uint8_t pacY, bool superPac);
    bool isAtPosition(uint8_t px, uint8_t py) const;
    uint8_t getX() const;
    uint8_t getY() const;
};

#endif
