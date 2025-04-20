#include "utils/DELAY.h"

void custom_delay_us(uint16_t microseconds) {
    for (uint16_t i = 0; i < microseconds; i++) {
        _delay_us(1);
    }
}

void custom_delay_ms(uint16_t milliseconds) {
    for (uint16_t i = 0; i < milliseconds; i++) {
        custom_delay_us(1000);
    }
}
