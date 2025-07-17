#include "game/Pacman.h"

Pacman::Pacman(
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
): 
    x(startX), 
    y(startY), 
    startX(startX), 
    startY(startY), 
    name(playerName),
    dot(0), 
    score(0), 
    counter(0), 
    superPac(0), 
    wakaToggle(false),
    displayMenu(displayMenu), 
    displayGame(displayGame),
    menuDrawn(menuDrawn),
    gameDrawn(gameDrawn),
    pinky(pinky), 
    blinky(blinky), 
    clyde(clyde) 
{}

void Pacman::reset() {
    x = startX;
    y = startY;
    dot = score = counter = superPac = 0;
    wakaToggle = false;
    pacmanMap[y][x] = PACMAN;

    pinky->reset(x, y, isSuperPac());
    blinky->reset(x, y, isSuperPac());
    clyde->reset(x, y, isSuperPac());
}

void Pacman::initializeGameBoard() {
    *displayGame = true;
    *displayMenu = false;
    *gameDrawn = true;
    *menuDrawn = false;

    loadMap(resetMap);
    drawMap();

    reset();
    draw();
}

void Pacman::initializeMenu() {
    *displayGame = false;
    *displayMenu = true;
    *gameDrawn = false;
    *menuDrawn = true;

    loadMap(menu);
    drawMap();
}

void Pacman::draw() {
    drawBlock(x, y, ST7735_YELLOW);
}

void Pacman::moveTo(uint8_t newX, uint8_t newY) {
    if (pacmanMap[newY][newX] == WALL) return;

    drawBlock(x, y, ST7735_BLACK);
    x = newX; y = newY;

    if (x >= MAP_WIDTH) x = 1;
    if (x <= 0)  x = MAP_WIDTH - 1;

    uint8_t tile = pacmanMap[y][x];

    if (!superPac && (tile == PINKY || tile == BLINKY || tile == CLYDE)) {
        loseGame();
        playMelody(pacmanLosingMelody, losingNotes);
        return;
    }

    if (tile == DOT || tile == POWER) {
        if (tile == POWER) superPac = 1;

        pacmanMap[y][x] = PATH;
        dot++;

        int index = wakaToggle ? 2 : 0;
        int note = pacmanPelletsMelody[index];
        int length = pacmanPelletsMelody[index + 1];
        int duration = wholenote / length;

        playNote(note, duration / 2);
        wakaToggle = !wakaToggle;
    }

    draw();
}

bool Pacman::checkGhostCollision() const {
    return !superPac &&
        (pinky->isAtPosition(x, y) ||
         blinky->isAtPosition(x, y) ||
         clyde->isAtPosition(x, y));
}

bool Pacman::collectedAllDots() const {
    return dot >= MAX_DOTS;
}

bool Pacman::isSuperPacExpired() {
    return superPac && counter++ >= SUPER_PAC_DURATION;
}

bool Pacman::isSuperPac() const {
    return superPac > 0;
}

void Pacman::resetSuperPac() {
    superPac = 0;
    counter = 0;
}

void Pacman::loseGame() {
    *displayMenu = true;
    *displayGame = false;
    *gameDrawn = false;
    *menuDrawn = false;
    score = 0;
}

void Pacman::winGame() {
    *displayMenu = true;
    *displayGame = false;
    *gameDrawn = false;
    *menuDrawn = false;
    score++;
}

uint8_t Pacman::getX() const { return x; }

uint8_t Pacman::getY() const { return y; }

uint8_t Pacman::getScore() const {
    return score;
}

