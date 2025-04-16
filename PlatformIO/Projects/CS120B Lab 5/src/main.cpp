/*
Your Name and Email: Tristan Cai (tcai019@ucr.edu)
Discussion Section: 024
Assignment: Lab 5 Part 3
Exercise Description:

I acknowledge all content contained herein, excluding template or example code,
is my own original work.

Demo Link:
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include "helper.h"
#include "timerISR.h"
#include "periph.h"

#define NUM_TASKS 5

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

task tasks[NUM_TASKS];

/*
Represent Periods for concurrent SynchSMs.
*/
const unsigned long sonarPeriod = 1000;
const unsigned long displayPeriod = 1;
const unsigned long redGreenPeriod = 1;
const unsigned long joystickPeriod = 200;
const unsigned long GCD_PERIOD = findGCD(sonarPeriod, displayPeriod);
/*
Helper variables for sonar, thresholds, and centimeter to inch conversion.
*/
volatile int distance_cm = 0;
volatile int display_unit = 1;
volatile int threshold_low = 4;
volatile int threshold_high = 9;
/*
Declaring all of the different enums and ticks.
*/
enum SonarStates { SONAR_MEASURE };
int Sonar_Tick(int state);

enum DisplayStates { DISPLAY_SHOW_DISTANCE };
int Display_Tick(int state);

enum RedLEDStates { RED_LED_CONTROL };
int RedLED_Tick(int state);

enum GreenLEDStates { GREEN_LED_CONTROL };
int GreenLED_Tick(int state);

enum JoystickStates { IDLE, UP, DOWN };
int Joystick_Tick(int state);
/*
TimerISR used for tasks.
*/
void TimerISR() {
    for (unsigned int i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].elapsedTime == tasks[i].period) {
            tasks[i].state = tasks[i].TickFct(tasks[i].state);
            tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += GCD_PERIOD;
    }
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    DDRC = 0xFC; PORTC = 0x03;
    int i = 0;
    
    ADC_init();
    init_sonar();
    /*
    Assign all tasks.
    */
    tasks[i].period = 1000;
    tasks[i].state = SONAR_MEASURE;
    tasks[i].elapsedTime = tasks[0].period;
    tasks[i].TickFct = &Sonar_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = DISPLAY_SHOW_DISTANCE;
    tasks[i].elapsedTime = tasks[1].period;
    tasks[i].TickFct = &Display_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = RED_LED_CONTROL;
    tasks[i].elapsedTime = tasks[2].period;
    tasks[i].TickFct = &RedLED_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = GREEN_LED_CONTROL;
    tasks[i].elapsedTime = tasks[3].period;
    tasks[i].TickFct = &GreenLED_Tick;
    ++i;
    tasks[i].period = 200;
    tasks[i].state = IDLE;
    tasks[i].elapsedTime = tasks[4].period;
    tasks[i].TickFct = &Joystick_Tick;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {}

    return 0;
}

/*
Sonar Tick function task.
*/
int Sonar_Tick(int state) {
    switch (state) { // State Transitions
        case SONAR_MEASURE:
            state = SONAR_MEASURE;
            break;
        default:
            state = SONAR_MEASURE;
            break;
    }
    switch (state) { // State Transitions
        case SONAR_MEASURE:
            distance_cm = (int)read_sonar();
            break;
        default:
            break;
    }
    return state;
}

/*
Display tick function task.
*/
int Display_Tick(int state) {
    static int current_digit = 0;
    static int digit_values[4] = {0};
    int distance = 0;
    
    switch (state) { // State Transitions
        case DISPLAY_SHOW_DISTANCE:
            state = DISPLAY_SHOW_DISTANCE;
            break;
        default:
            state = DISPLAY_SHOW_DISTANCE;
            break;
    }
    switch (state) { // State Actions
        case DISPLAY_SHOW_DISTANCE:
            if (display_unit) {
                distance = distance_cm;
            } else {
                distance = distance_cm * 100 / 254;
            }
            
            for (int i = 0; i < 4; i++) {
                digit_values[i] = distance % 10;
                distance /= 10;
            }

            switch (current_digit) {
                case 0:
                    PORTB |= ((1 << PB3) | (1 << PB4) | (1 << PB5));
                    PORTB &= ~(1 << PB2);
                    outNum(digit_values[0]);
                    break;
                case 1:
                    PORTB |= ((1 << PB2) | (1 << PB4) | (1 << PB5));
                    PORTB &= ~(1 << PB3);
                    outNum(digit_values[1]);
                    break;
                case 2:
                    PORTB |= ((1 << PB2) | (1 << PB3) | (1 << PB5));
                    PORTB &= ~(1 << PB4);
                    outNum(digit_values[2]);
                    break;
                case 3:
                    PORTB |= ((1 << PB2) | (1 << PB3) | (1 << PB4));
                    PORTB &= ~(1 << PB5);
                    outNum(digit_values[3]);
                    break;
            }

            current_digit = (current_digit + 1) % 4;
            break;
        default:
            break;
    }
    return state;
}

/*
RedLED tick function task.
*/
int RedLED_Tick(int state) {
    static unsigned int red_count = 0;
    static unsigned int red_period = 10;
    static unsigned int red_duty_cycle = 0;

    switch (state) { // State Transitions
        case RED_LED_CONTROL:
            state = RED_LED_CONTROL;
            break;
        default:
            state = RED_LED_CONTROL;
            break;
    }
    switch (state) { // State Actions
        case RED_LED_CONTROL:
            if (distance_cm < threshold_low) {
                red_duty_cycle = 100;
            } else if (distance_cm >= threshold_low && distance_cm <= threshold_high) {
                red_duty_cycle = 90;
            } else {
                red_duty_cycle = 0;
            }

            if (red_count < red_duty_cycle) {
                PORTC |= (1 << PC5);
            } else {
                PORTC &= ~(1 << PC5);
            }

            red_count = (red_count + 1) % red_period;
            break;
        default:
            break;
    }
    return state;
}

/*
GreenLED tick function task.
*/
int GreenLED_Tick(int state) {
    static unsigned int green_count = 0;
    static unsigned int green_period = 10;
    static unsigned int green_duty_cycle = 0;

    switch (state) { // State Transitions
        case GREEN_LED_CONTROL:
            state = GREEN_LED_CONTROL;
            break;
        default:
            state = GREEN_LED_CONTROL;
            break;
    }
    switch (state) { // State Actions
        case GREEN_LED_CONTROL:
            if (distance_cm < threshold_low) {
                green_duty_cycle = 0;
            } else if (distance_cm >= threshold_low && distance_cm <= threshold_high) {
                green_duty_cycle = 30;
            } else {
                green_duty_cycle = 100;
            }

            if (green_count < green_duty_cycle) {
                PORTC |= (1 << PC4);
            } else {
                PORTC &= ~(1 << PC4);
            }

            green_count = (green_count + 1) % green_period;
            break;
        default:
            break;
    }
    return state;
}

/*
Joystick tick function task.
*/
int Joystick_Tick(int state) {
    static unsigned char prev_button = 0;
    unsigned char button = ~PINC & (1 << PC1);
    
    switch (state) {
        case IDLE:  
            if (button && !prev_button) {
                display_unit = !display_unit;
            }
            if (ADC_read(0) < 300) {
                state = UP;
            } else if (ADC_read(0) > 700) {
                state = DOWN;
            }

            prev_button = button;
            break;
        case UP:
            state = IDLE;
            break;
        case DOWN:
            state = IDLE;
            break;
        default:
            state = IDLE;
            break;
    } 
    switch (state) {
        case IDLE:
            if (button && !prev_button) {
                display_unit = !display_unit;
            }
            prev_button = button;
            break;
        case UP:
            if (threshold_high == 19) {
                threshold_low = 4;
                threshold_high = 9;
            } else if (threshold_high == 29) {
                threshold_low = 9;
                threshold_high = 19;
            }
            break;
        case DOWN:
            if (threshold_high == 9) {
                threshold_low = 9;
                threshold_high = 19;
            } else if (threshold_high == 19) {
                threshold_low = 14;
                threshold_high = 29;
            }
            break;
        default:
            break;
    }    
    return state;
}