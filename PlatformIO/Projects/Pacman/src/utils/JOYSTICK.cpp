#include "utils/JOYSTICK.h"

uint8_t joystickUp = 0;
uint8_t joystickDown = 0;
uint8_t joystickLeft = 0;
uint8_t joystickRight = 0;
uint8_t joystickBtn = 0;

void updateJoystickState() {
    joystickBtn = !GetBit(PINF, 2);
    joystickUp = ADC_READ(0) > 600;
    joystickDown = ADC_READ(0) < 400;
    joystickRight = ADC_READ(1) > 600;
    joystickLeft = ADC_READ(1) < 400;
}