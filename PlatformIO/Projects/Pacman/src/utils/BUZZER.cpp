#include "utils/Buzzer.h"

void buzzer_on() {
    PORTH = SetBit(PORTH, PH3, 1);
}

void buzzer_off() {
    PORTH = SetBit(PORTH, PH3, 0);
}