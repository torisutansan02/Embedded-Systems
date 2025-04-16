#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timerISR.h"

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
    return (b ? (x | (0x01 << k)) : (x & ~(0x01 << k)));
}

unsigned char GetBit(unsigned char x, unsigned char k) {
    return ((x & (0x01 << k)) != 0);
}

void ADC_init() {
    ADMUX = (1 << REFS0);
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

unsigned int ADC_read(unsigned char chnl) {
    uint8_t low, high;

    ADMUX = (ADMUX & 0xF8) | (chnl & 7);
    ADCSRA |= 1 << ADSC;
    while ((ADCSRA >> ADSC) & 0x01) {}

    low = ADCL;
    high = ADCH;

    return ((high << 8) | low);
}

int nums[16] = {0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011, 0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011, 0b1110111, 0b0011111, 0b1001110, 0b0111101, 0b1001111, 0b1000111};
void outNum(int num) {
    PORTD = nums[num] << 1; //assigns bits 1-7 of nums(a-f) to pins 2-7 of port d
    PORTB = SetBit(PORTB, 0, nums[num] & 0x01); // assigns bit 0 of nums(g) to pin 0 of port b
}

int directions[4] = {0b0111110, 0b0111101, 0b0001110, 0b0000101}; 
void outDir(int dir) {
    PORTD = directions[dir] << 1;
    PORTB = SetBit(PORTB, 0, directions[dir] & 0x01);
}

int passcode[4] = {3, 2, 1, 2};
int entered_code[4] = {0, 0, 0, 0};
int phases[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b100, 0b1001};

enum states {IDLE, LOCKED, UNLOCKED} state;
int num = 0;
int i = 0;
int counter = 0;
int timer = 0;
bool joystick_pull_release = false;
bool locked = true;
bool newPassword = false;

void Tick() {
    // State Transitions
    switch(state) {
        case IDLE:
            if (ADC_read(0) < 300 || ADC_read(0) > 700 || ADC_read(1) < 300 || ADC_read(1) > 700) { 
                joystick_pull_release = true;
            } else {
                if (joystick_pull_release) {
                    i++;
                    if (i > 3) { 
                        entered_code[0] = 0;
                        entered_code[1] = 0;
                        entered_code[2] = 0;
                        entered_code[3] = 0;
                        newPassword = false;
                        i = 0;
                    }
                    joystick_pull_release = false;
                }
            }

            if (!locked && (PINC & (1 << PC2)) && i == 0) {
                newPassword = true;
            } else {
                PORTC |= (1 << PC4);
            }

            if (ADC_read(0) < 300) { 
                num = 0;
                if (!newPassword) {
                    entered_code[i] = num; 
                }
                else if (newPassword) {
                    passcode[i] = num;
                }
                if (locked)
                 state = LOCKED; 
                else
                 state = UNLOCKED;
            } else if (ADC_read(0) > 700) { 
                num = 1; 
                if (!newPassword) {
                    entered_code[i] = num; 
                }
                else if (newPassword) {
                    passcode[i] = num;
                }
                if (locked)
                 state = LOCKED; 
                else
                 state = UNLOCKED;
            } else if (ADC_read(1) > 700) { 
                num = 2; 
                if (!newPassword) {
                    entered_code[i] = num; 
                }
                else if (newPassword) {
                    passcode[i] = num;
                }
                if (locked)
                 state = LOCKED; 
                else
                 state = UNLOCKED;
            } else if (ADC_read(1) < 300) { 
                num = 3; 
                if (!newPassword) {
                    entered_code[i] = num; 
                }
                else if (newPassword) {
                    passcode[i] = num;
                }
                if (locked)
                 state = LOCKED;  
                else if (!locked)
                 state = UNLOCKED;
            }
            break;

        case LOCKED:
            if (entered_code[0] == passcode[0] &&
                entered_code[1] == passcode[1] &&
                entered_code[2] == passcode[2] &&
                entered_code[3] == passcode[3]) {
                if (i > 2) {
                    locked = false;
                    timer = 0;
                }
                state = UNLOCKED;
            } else { 
                state = IDLE; 
            }
            break;

        case UNLOCKED:
            if (entered_code[0] != passcode[0] ||
                entered_code[1] != passcode[1] ||
                entered_code[2] != passcode[2] ||
                entered_code[3] != passcode[3]) {
                if (i > 2) {
                    locked = true;
                    timer = 0;
                }
                state = LOCKED;
            } else { 
                state = IDLE; 
            }
            break;

        default:
            state = IDLE;
            break;
    }

    // State Actions
    switch(state) {
        case IDLE:
            if (!locked && newPassword) {
                PORTC &= ~(1 << PC4);
            }
            if (joystick_pull_release) {
                outDir(num);
            } else {
                outNum(i);
                if (locked && timer <= 1024) {
                  PORTB = (PORTB & 0x03) | phases[counter] << 2;
                  counter++;
                  timer++;
                  if (counter > 7) {
                    counter = 0;
                  }
                }
                else if (!locked && timer <= 1024) {
                  PORTB = (PORTB & 0x03) | phases[counter] << 2;
                  timer++;
                  counter--;
                  if (counter < 0) {
                    counter = 7;
                  }
                }
            }
            break;

        case LOCKED:
            PORTC |= (1 << PC5);
            break;

        case UNLOCKED:
            PORTC &= ~(1 << PC5);
            break;

        default:
            break;
    }
}

int main(void) {
    DDRB = 0xFF;
    DDRD = 0xFF;
    DDRC = 0b0110100;

    ADC_init();

    state = IDLE;

    TimerSet(1); // Period of 1 ms
    TimerOn(); // Start timer

    while (1) {
        Tick(); // Execute one tick
        while (!TimerFlag) {} // Wait for timer period
        TimerFlag = 0; // Lower flag
    }

    return 0;
}
