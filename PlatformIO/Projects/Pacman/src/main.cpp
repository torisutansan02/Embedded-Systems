#include "utils/ISR.h"

#include "game/Ghost.h"
#include "game/Sound.h"
#include "game/Display.h"
#include "game/Pacman.h"

/*
Game Declarations
*/

bool displayMenu = true;
bool displayGame = false;
bool menuDrawn = false;
bool gameDrawn = false;

Ghost pinky(PINKY, 11, 9);
Ghost blinky(BLINKY, 11, 10);
Ghost clyde(CLYDE, 11, 11);

Pacman pacman(PACMAN, 14, 29, &displayMenu, &displayGame, &menuDrawn, &gameDrawn, &pinky, &blinky, &clyde);

task tasks[NUM_TASKS];

/*
Declare State Machines and Tick Functions
*/

enum JoystickState { JS_INIT, JS_READ };
int Joystick_Tick(int state);

enum GameState { GS_INIT, GS_RUNNING };
int Game_Tick(int state);

enum DisplayTick{ DS_INIT, DS_SHOW };
int Display_Tick(int state);

enum PacmanState { PS_INIT, PS_MOVE };
int Pacman_Tick(int state);

enum EnemyState { ES_INIT, ES_CHASE };
int Enemy_Tick(int state);

/*
Initialize Hardware
*/

int main(void) {
    DDRB = DDRD = DDRH = DDRE = 0xFF;
    DDRF = 0x00;
    PORTB = PORTD = PORTH = PORTE = 0x00;
    PORTF = 0xFF;

    ADC_INIT();
    SPI_INIT();
    ST7735_init();

    unsigned long p0 = 1;
    unsigned long p1 = 1;
    unsigned long p2 = 150;
    unsigned long p3 = 150;
    unsigned long p4 = 3;
    
    unsigned long GCD = findGCD(findGCD(p0, p1), findGCD(p2, p3));
    
    tasks[0] = (task){ JS_INIT, p0 / GCD, 0, &Joystick_Tick };
    tasks[1] = (task){ GS_INIT, p1 / GCD, 0, &Game_Tick };
    tasks[2] = (task){ ES_INIT, p2 / GCD, 0, &Enemy_Tick };
    tasks[3] = (task){ PS_INIT, p3 / GCD, 0, &Pacman_Tick };
    tasks[4] = (task){ DS_INIT, p4 / GCD, 0, &Display_Tick };

    DDRJ |= (1 << PJ0);   // Set PJ0 as output
    PORTJ &= ~(1 << PJ0); // Clear initially

    TimerSet(GCD);
    TimerOn();

    while (1);
}

/*
Tick Function and State Machine Logic
*/

int Joystick_Tick(int state) {
    switch (state) {
        case JS_INIT: 
            state = JS_READ; 
            break;
        case JS_READ: 
            break;
        default: 
            state = JS_INIT; 
            break;
    }
    switch (state) {
        case JS_INIT: 
            break;
        case JS_READ: 
            updateJoystickState(); 
            break;
        default: 
            break;
    }
    return state;
}

int Game_Tick(int state) {
    switch (state) {
        case GS_INIT: 
            if (joystickBtn) {
                state = GS_RUNNING;
            }
            break;
        case GS_RUNNING: 
            if (!displayGame) {
                state = GS_INIT; 
            }
            break;
        default: 
            state = GS_INIT; 
            break;
    }
    switch (state) {
        case GS_INIT:
            if (displayMenu && !menuDrawn) {
                pacman.initializeMenu();
            }
            break;
        case GS_RUNNING:
            displayGame = true;
            if (displayGame && !gameDrawn) {
                pacman.initializeGameBoard();
                playMelody(melody, notes);
            }
            break;
        default: 
            break;
    }
    return state;
}

int Display_Tick(int state) {
    static uint8_t digitIndex = 0;
    static int digits[4];

    switch (state) {
        case DS_INIT:
            if (displayGame) {
                state = DS_SHOW;
            }
            break;
        case DS_SHOW:
            if (!displayGame) {
                state = DS_INIT;
            }
            break;
        default:
            state = DS_INIT;
            break;
    }

    switch (state) {
        case DS_INIT:
            break;

        case DS_SHOW: {
            int score = pacman.getScore();

            // Break score into digits
            digits[0] = (score / 1000) % 10;
            digits[1] = (score / 100) % 10;
            digits[2] = (score / 10) % 10;
            digits[3] = score % 10;

            // Optional: blank leading zeros
            bool leadingZero = true;
            for (int i = 0; i < 4; i++) {
                if (digits[i] == 0 && leadingZero && i != 3) {
                    digits[i] = 10;  // index 10 = blank
                } else {
                    leadingZero = false;
                }
            }

            // Turn off all digits (set HIGH)
            PORTL |= (1 << PL4) | (1 << PL5) | (1 << PL6) | (1 << PL7);

            // Output current digit's segment pattern
            outNum(digits[digitIndex]);

            // Enable one digit (active LOW)
            switch (digitIndex) {
                case 0: PORTL &= ~(1 << PL7); break; // DIG1 (leftmost)
                case 1: PORTL &= ~(1 << PL6); break; // DIG2
                case 2: PORTL &= ~(1 << PL5); break; // DIG3
                case 3: PORTL &= ~(1 << PL4); break; // DIG4 (rightmost)
            }

            // Move to next digit for next tick
            digitIndex = (digitIndex + 1) % 4;
            break;
        }

        default:
            break;
    }

    return state;
}

int Pacman_Tick(int state) {
    switch (state) {
        case PS_INIT: 
            if (displayGame) {
                state = PS_MOVE;
            }
        break;
        case PS_MOVE: 
            if (!displayGame) {
                state = PS_INIT;
            }
            break;
        default: 
            state = PS_INIT; 
            break;
    }
    switch (state) {
        case PS_INIT: 
            break;
        case PS_MOVE:
            if (joystickUp) {
                pacman.moveTo(pacman.getX(), pacman.getY() - 1);
            }
            if (joystickDown) {
                pacman.moveTo(pacman.getX(), pacman.getY() + 1);
            }
            if (joystickRight) { 
                pacman.moveTo(pacman.getX() + 1, pacman.getY());
            }
            if (joystickLeft) {
                pacman.moveTo(pacman.getX() - 1, pacman.getY());
            }

            if (pacman.checkGhostCollision()) {
                pacman.loseGame();
                playMelody(pacmanLosingMelody, losingNotes);
            }

            if (pacman.collectedAllDots())  {
                pacman.winGame();
            }

            if (pacman.isSuperPacExpired()) {
                pacman.resetSuperPac();
            };
            break;
        default: break;
    }
    return state;
}

int Enemy_Tick(int state) {
    switch (state) {
        case ES_INIT: 
            if (displayGame) {
                state = ES_CHASE;
            }
            break;
        case ES_CHASE: 
            if (!displayGame) {
                state = ES_INIT;
                break;
            }
        default: 
            state = ES_INIT; 
            break;
    }
    switch (state) {
        case ES_INIT: 
            break;
        case ES_CHASE:
            pinky.update(pacman.getX(), pacman.getY(), pacman.isSuperPac());
            blinky.update(pacman.getX(), pacman.getY(), pacman.isSuperPac());
            clyde.update(pacman.getX(), pacman.getY(), pacman.isSuperPac());
            break;
        default: 
            break;
    }
    return state;
}