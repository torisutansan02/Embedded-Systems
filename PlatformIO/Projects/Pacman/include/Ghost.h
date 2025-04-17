#ifndef GHOST_H
#define GHOST_H

#include <stdint.h>

class Ghost {
private:
    uint8_t x, y;
    uint8_t startX, startY;
    uint8_t type;
    uint8_t underTile; // 👈 Add this

public:
    Ghost(uint8_t ghostType, uint8_t startX, uint8_t startY);

    void update(uint8_t pacX, uint8_t pacY);
    void move(uint8_t pacX, uint8_t pacY);
    void reset();
    void drawGhost();
    void eraseGhost();
    bool isAtPosition(uint8_t px, uint8_t py) const;
    uint8_t getX() const;
    uint8_t getY() const;
};

#endif
