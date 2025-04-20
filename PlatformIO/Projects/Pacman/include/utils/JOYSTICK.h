#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

#include "utils/ADC.h"
#include "utils/BIT.h"

extern uint8_t joystickUp;
extern uint8_t joystickDown;
extern uint8_t joystickLeft;
extern uint8_t joystickRight;
extern uint8_t joystickBtn;

void updateJoystickState();

#endif