#ifndef PACMAN_H
#define PACMAN_H

#include "game/Display.h"
#include "game/Sound.h"
#include "game/Constants.h"
#include "game/Ghost.h"
#include "utils/JOYSTICK.h"
#include "utils/FOURD7S.h"

class Ghost;

#include <stdint.h>

extern uint8_t pacmanMap[MAP_HEIGHT][MAP_WIDTH];

class Pacman {
private:
    uint8_t x, y;
    uint8_t startX, startY;
    uint8_t name;
    uint8_t dot, score, counter, superPac;
    bool wakaToggle;

    bool* displayMenu;
    bool* displayGame;
    bool* menuDrawn;
    bool* gameDrawn;

    Ghost* pinky;
    Ghost* blinky;
    Ghost* clyde;

public:
    Pacman(
        uint8_t playerName, 
        uint8_t startX, 
        uint8_t startY,
        bool* displayMenu, 
        bool* displayGame,
        bool* menuDrawn, 
        bool* gameDrawn,
        Ghost* pinky, 
        Ghost* blinky, 
        Ghost* clyde
    );

    void reset();
    void initializeGameBoard();
    void initializeMenu();
    void moveTo(uint8_t newX, uint8_t newY);
    void draw();

    bool checkGhostCollision() const;
    bool collectedAllDots() const;
    bool isSuperPacExpired();
    void resetSuperPac();

    void loseGame();
    void winGame();

    uint8_t getX() const;
    uint8_t getY() const;
    uint8_t getScore() const;
    bool isSuperPac() const;
};

#endif
