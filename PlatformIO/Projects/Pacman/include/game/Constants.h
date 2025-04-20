#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
C STD Library Headers
*/
#include <stdint.h>

enum TileType : uint8_t {
    PATH = 0,
    WALL = 1,
    DOT = 2,
    PACMAN = 3,
    PINKY = 4,
    BLINKY = 5,
    CLYDE = 6,
    POWER = 7
};

constexpr uint8_t MAP_WIDTH  = 32;
constexpr uint8_t MAP_HEIGHT = 32;
constexpr uint8_t MAX_DOTS = 61;
constexpr uint8_t SUPER_PAC_DURATION = 100;

#endif
