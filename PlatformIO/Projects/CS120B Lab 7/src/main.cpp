#include "timerISR.h"
#include "helper.h"
#include "periph.h"

#define NUM_TASKS 5

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

const unsigned long GCD_PERIOD = 1;
unsigned char buzzerState = 1;
unsigned char buzzerOffFlag = 1;

task tasks[NUM_TASKS];

enum Horn { IDLE, BUZZER };
int Horn_Tick(int state);

enum Indicator { WAIT, RIGHT, LEFT };
int Indicator_Tick(int state);

enum StepperMotor { HALT, UP, DOWN };
int Drive_Tick(int state);

enum BuzzerState { BUZZER_OFF, BUZZER_ON };
int Buzzer_Tick(int state);

enum ServoMotor { STOP, LEFT_S, RIGHT_S };
int Steer_Tick(int state);

void TimerISR() {
    for (unsigned int i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime == tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}

int stages[8] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001};

int main(void) {
    DDRB = 0xFF;
    DDRD = 0xFF;
    PORTB = 0x00;
    PORTD = 0x00;
    PORTC = 0b011111;

    TCCR0A |= (1 << COM0A1);
    TCCR0A |= (1 << WGM01) | (1 << WGM00);
    TCCR0B = (TCCR0B & 0xF8);
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
    ICR1 = 39999; 
    ADC_init();

    int i = 0;

    tasks[i].period = 1;
    tasks[i].state = IDLE;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Horn_Tick;
    ++i;
    tasks[i].period = 10;
    tasks[i].state = WAIT;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Indicator_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = HALT;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Drive_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = BUZZER_OFF;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Buzzer_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = STOP;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Steer_Tick;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {}

    return 0;
}

int Horn_Tick(int state) {
    switch(state) { // State Transitions
        case IDLE:
            if (GetBit(PINC, 2) == 0) {
                state = BUZZER;
                buzzerOffFlag = 0;
            } else {
                state = IDLE;
            }
            break;
        case BUZZER:
            if (GetBit(PINC, 2) == 1) {
                state = IDLE;
                buzzerOffFlag = 1;
            } else {
                state = BUZZER;
            }
            break;
        default:
            state = IDLE;
            break;
    }
    switch(state) { // State Actions
        case IDLE:
            if (!buzzerOffFlag) {
                OCR0A = 255;
            }
            break;
        case BUZZER:
            OCR0A = 128;
            TCCR0B = (TCCR0B & 0xF8) | 0x04;
            break;
        default:
            break;
    }
    return state;
}

int Indicator_Tick(int state) {
    static unsigned char counter = 0;
    switch(state) { // State Transitions
        case WAIT:
            if (PINC & (1 << PC3)) {
                counter = 0;
                state = LEFT;
            } else if (PINC & (1 << PC4)) {
                counter = 0;
                state = RIGHT;
            } else {
                state = WAIT;
            }
            break;
        case RIGHT:
            if (!(PINC & (1 << PC4))) {
                counter = 0;
                state = WAIT;
            }
            if (counter <= 50) {
                PORTD = SetBit(PORTD, 4, 0);
                PORTD = SetBit(PORTD, 3, 0);
                PORTD = SetBit(PORTD, 2, 0);
            } else if (counter <= 100) {
                PORTD = SetBit(PORTD, 4, 1);
                PORTD = SetBit(PORTD, 3, 0);
                PORTD = SetBit(PORTD, 2, 0);
            } else if (counter <= 200) {
                PORTD = SetBit(PORTD, 4, 1);
                PORTD = SetBit(PORTD, 3, 1);
                PORTD = SetBit(PORTD, 2, 0);
            } else if (counter <= 300) {
                PORTD = SetBit(PORTD, 4, 1);
                PORTD = SetBit(PORTD, 3, 1);
                PORTD = SetBit(PORTD, 2, 1);
            } else {
                counter = 0;
                state = WAIT;
            }
            ++counter;
            break;
        case LEFT:
            if (!(PINC & (1 << PC3))) {
                counter = 0;
                state = WAIT;
            }
            if (counter <= 50) {
                PORTD = SetBit(PORTD, 5, 0);
                PORTD = SetBit(PORTD, 7, 0);
                PORTB = SetBit(PORTB, 0, 0);
            } else if (counter <= 100) {
                PORTD = SetBit(PORTD, 5, 0);
                PORTD = SetBit(PORTD, 7, 0);
                PORTB = SetBit(PORTB, 0, 1);
            } else if (counter <= 200) {
                PORTD = SetBit(PORTD, 5, 0);
                PORTD = SetBit(PORTD, 7, 1);
                PORTB = SetBit(PORTB, 0, 1);
            } else if (counter <= 300) {
                PORTD = SetBit(PORTD, 5, 1);
                PORTD = SetBit(PORTD, 7, 1);
                PORTB = SetBit(PORTB, 0, 1);
            } else {
                counter = 0;
                state = WAIT;
            }
            ++counter;
            break;
        default:
            state = WAIT;
            break;
    }
    switch(state) { // State Actions
        case WAIT:
            PORTD = SetBit(PORTD, 5, 0);
            PORTD = SetBit(PORTD, 7, 0);
            PORTB = SetBit(PORTB, 5, 0);

            PORTD = SetBit(PORTD, 4, 0);
            PORTD = SetBit(PORTD, 3, 0);
            PORTD = SetBit(PORTD, 2, 0);
            break;
        case RIGHT:
            break;
        case LEFT:
            break;
        default:
            break;
    }
    return state;
}

int Drive_Tick(int state) {
    static unsigned char i = 0;
    static unsigned char upCounter = 0;
    static unsigned char downCounter = 0;
    static unsigned char upCount = 30;
    static unsigned char downCount = 0;

    switch(state) { // State Transitions
        case HALT:
            if (ADC_read(0) > 700) {
                state = UP;
            } else if (ADC_read(0) < 300) {
                buzzerState = 0;
                state = DOWN;
            } else {
                state = HALT;
            }
            break;
        case UP:
            if (ADC_read(0) < 700) {
                state = HALT;
            } else {
                state = UP;
            }
            break;
        case DOWN:
            if (ADC_read(0) > 300) {
                state = HALT;
                buzzerState = 1;
            }
            break;
        default:
            buzzerState = 1;
            state = HALT;
            break;
    }
    switch (state) { // State Actions
        case HALT:
            break;
        case UP:
            upCounter = map_value(700, 825, 2, 30, ADC_read(0));
            if (--upCount <= upCounter) {
                PORTB = (PORTB & 0x03) | (stages[i] << 2);
                i = (i == 0) ? (8 - 1) : (i - 1);
                upCount = 30;
            }
            break;
        case DOWN:
            downCounter = map_value(0, 300, 2, 30, ADC_read(0));
            if (++downCount >= downCounter) {
                PORTB = (PORTB & 0x03) | (stages[i] << 2);
                i = (i + 1) % 8;
                downCount = 0;
            }
            break;
        default:
            break;
    }

    return state;
}

int Buzzer_Tick(int state) {
    static unsigned long counter = 0;
    switch(state) { // State Transitions
        case BUZZER_OFF:
            if (buzzerState == 0 && counter < 2000) {
                counter++;
            } else if (buzzerState == 0 && counter >= 2000) {
                counter = 0;
                state = BUZZER_ON;
            }
            break;
        case BUZZER_ON:
            if (counter < 1000) {
                counter++;
            } else {
                counter = 0;
                state = BUZZER_OFF;
            }
            break;
        default:
            counter = 0;
            state = BUZZER_OFF;
            break;
    }
    switch(state) { // State Actions
        case BUZZER_OFF:
            if (buzzerOffFlag) {
                OCR0A = 255;
            }
            break;
        case BUZZER_ON:
            OCR0A = 128;
            TCCR0B = (TCCR0B & 0xF8) | 0x03;
            break;
        default:
            break;
    }
    return state;
}

int Steer_Tick(int state) {
    switch (state) { // State Transitions
        case STOP:
            if (ADC_read(1) > 513) {
                state = RIGHT_S;
            } else if (ADC_read(1) < 511) {
                state = LEFT_S;
            } else {
                state = STOP;
            }
            break;
        case RIGHT_S:
            if (ADC_read(1) <= 513) {
                state = STOP;
            }
            break;
        case LEFT_S:
            if (ADC_read(1) >= 511) {
                state = STOP;
            }
            break;
        default:
            state = STOP;
            break;
    }
    switch (state) { // State Actions
        case STOP:
            OCR1A = 3000;
            break;
        case RIGHT_S:
            OCR1A = map_value(513, 825, 3000, 5000, ADC_read(1));
            break;
        case LEFT_S:
            OCR1A = map_value(0, 511, 1000, 3000, ADC_read(1));
            break;
        default:
            break;
    }

    return state;
}