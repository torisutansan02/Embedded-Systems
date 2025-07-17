#include "utils/FOURD7S.h"

int nums[11] = {
    0b1111110, // 0 → a b c d e f     (g=0)
    0b0110000, // 1 →   b c
    0b1101101, // 2 → a b   d e   g
    0b1111001, // 3 → a b c d     g
    0b0110011, // 4 →   b c     f g
    0b1011011, // 5 → a   c d   f g
    0b1011111, // 6 → a   c d e f g
    0b1110000, // 7 → a b c
    0b1111111, // 8 → all
    0b1111011, // 9 → a b c d   f g
    0b0000000  // 10 → blank
};


void outNum(int num) {
	PORTD = nums[num] << 1;               // PD1–PD7 (segments B–G), PD7 = unused
	PORTJ = SetBit(PORTJ, 0, nums[num] & 0x01); // PJ0 = segment A
}