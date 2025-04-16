
/*
Your Name and Email: Tristan Cai (tcai019@ucr.edu)
Discussion Section: 024
Assignment: Lab 6 Part 3
Exercise Description:

I acknowledge all content contained herein, excluding template or example code,
is my own original work.

Demo Link: https://www.youtube.com/watch?v=ocsfUxZw0Ss
*/

#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "LCD.h"
#include "serialATmega.h"

#define NUM_TASKS 4

typedef struct _task{
	signed 	 char state;
	unsigned long period;
	unsigned long elapsedTime;
	int (*TickFct)(int);
} task;
/*
Helper variables for dutyCycle, temps, thresholds, and Kelvin to Fahrenheight conversion.
*/
unsigned char dutyCycle = 0;
unsigned char fanRunTime = 0;
unsigned char fanOffTime = 0;
unsigned char dutyCycleActivate = 0;
unsigned char thermistorCycleActivate = 0;
static unsigned char modeDisplayed = 0;
static unsigned char mode = 0;
double tempF = 0;
int threshTemp = 90;
char fanRunTimeString[6];
char tempStr[6];
char threshStr[6];
const unsigned long GCD_PERIOD = 1;
const char *str1 = "Mode: MANUAL";
const char *str2 = "Mode: AUTO";
const char *str3 = "FAN: ";
const char *str4 = "Curr:";
const char *str5 = "Th:";
/*
Create Task Structure
*/
task tasks[NUM_TASKS];
/*
Declaring all of the different enums and tick functions.
*/
enum Buzzer { IDLE, BUZZER };
int Buzzer_Tick(int state);

enum DCMotor { DC_MOTOR };
int DCMotor_Tick(int state);

enum Thermistor { THERMISTOR };
int Thermistor_Tick(int state);

enum LCDScreen { LCD_SCREEN };
int LCDScreen_Tick(int state);
/*
TimerISR used for tasks.
*/
void TimerISR() {
	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {
		if ( tasks[i].elapsedTime == tasks[i].period ) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += GCD_PERIOD;
	}
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    PORTC = 0b000111;
    int i = 0;

    ADC_init();
    init_sonar();

    lcd_init();
    lcd_clear();
    /*
    Assign all tasks.
    */
    tasks[i].period = 1;
    tasks[i].state = BUZZER;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Buzzer_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = DC_MOTOR;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &DCMotor_Tick;
    ++i;
    tasks[i].period = 1000;
    tasks[i].state = THERMISTOR;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &Thermistor_Tick;
    ++i;
    tasks[i].period = 1;
    tasks[i].state = LCD_SCREEN;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &LCDScreen_Tick;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {}

    return 0;
}
/*
Tick function for the buzzer.
*/
int Buzzer_Tick(int state) {
    static unsigned int buzzerDuration = 100;
    static unsigned char prevJoystickState = 0;
    unsigned char joystickState = (ADC_read(1) < 100 || ADC_read(1) > 900 || !(PINC & (1 << PC2))) ? 1 : 0;
    unsigned char joystickChanged = (joystickState != prevJoystickState);
    switch(state) { // State Transitions
        case IDLE:
            if (joystickChanged) {
                state = BUZZER;
                buzzerDuration = 100;
            }
            else {
                state = IDLE;
            }
            break;
        case BUZZER:
            if (buzzerDuration > 0) {
                state = BUZZER;
            }
            else {
                state = IDLE;
            }
            break;
        default:
            state = IDLE;
            break;
    }
    switch (state) { // State Actions
        case BUZZER:
            PORTB |= (1 << PB0);
            buzzerDuration--;
            break;
        default:
            PORTB &= ~(1 << PB0);
            break;
    }
    prevJoystickState = joystickState;
    return state;
}
/*
Tick function for the DC Motor, manual mode.
*/
int DCMotor_Tick(int state) {
    static unsigned char prevUpState = 0;
    static unsigned char prevDownState = 0;
    unsigned char upState = (ADC_read(1) > 900) ? 1 : 0;
    unsigned char downState = (ADC_read(1) < 100) ? 1 : 0;
    unsigned char upChanged = (upState != prevUpState);
    unsigned char downChanged = (downState != prevDownState);
    switch (state) {
        case DC_MOTOR:
            state = DC_MOTOR;
            break;
        default:
            state = DC_MOTOR;
            break;
    }
    switch(state) {
        case DC_MOTOR:
            if (mode == 0) {
                if (upChanged && upState) {
                    if (dutyCycle < 10) {
                        dutyCycle += 1;
                        modeDisplayed = 0;
                        dutyCycleActivate = 1;
                    }
                } 
                if (downChanged && downState) {
                    if (dutyCycle > 0) {
                        dutyCycle -= 1;
                        modeDisplayed = 0;
                        dutyCycleActivate = 1;
                    }
                } 
                fanRunTime = (dutyCycle * 10);
                fanOffTime = (10 - dutyCycle) * 10;
                if (fanRunTime > 0) {
                    PORTB = SetBit(PORTB, 2, 1);
                    fanRunTime--;
                } 
                else if (fanOffTime > 0) {
                    PORTB = SetBit(PORTB, 2, 0);
                    fanOffTime--;
                }
            }
            else {
                dutyCycle = 0;
            }
            break;
        default:
            break;
    }
    prevUpState = upState;
    prevDownState = downState;
    return state;
}
/*
Tick function for the Thermistor.
*/
int Thermistor_Tick(int state) {
    int tempReading = ADC_read(0);
    double tempK = log(10000.0 * ((1024.0 / tempReading) - 1));
    switch(state) { // State Transitions
        case THERMISTOR:
            state = THERMISTOR;
        default:
            state = THERMISTOR;
            break;
    }
    switch(state) { // State Actions
        case THERMISTOR:
            if (mode == 1) {
                tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);
                tempF = ((9.0 / 5) * (tempK - 273.15)) + 32;
                modeDisplayed = 0;
                thermistorCycleActivate = 1;
                if (((int)tempF - threshTemp) == 1) {
                    dutyCycle = 1;
                }
                else if (((int)tempF - threshTemp) == 2) {
                    dutyCycle = 2;
                }
                else if (((int)tempF - threshTemp) == 3) {
                    dutyCycle = 3;
                }
                else if (((int)tempF - threshTemp) == 4) {
                    dutyCycle = 4;
                }
                else if (((int)tempF - threshTemp) == 5) {
                    dutyCycle = 5;
                }
                else if (((int)tempF - threshTemp) == 6) {
                    dutyCycle = 6;
                }
                else if (((int)tempF - threshTemp) == 7) {
                    dutyCycle = 7;
                }
                else if (((int)tempF - threshTemp) == 8) {
                    dutyCycle = 8;
                }
                else if (((int)tempF - threshTemp) == 9) {
                    dutyCycle = 9;
                }
                else if (((int)tempF - threshTemp) >= 10) {
                    dutyCycle = 10;
                }
                else {
                    dutyCycle = 0;
                }
                fanRunTime = (dutyCycle * 10);
                fanOffTime = (10 - dutyCycle) * 10;
                if (fanRunTime > 0) {
                    PORTB = SetBit(PORTB, 2, 1);
                    fanRunTime--;
                } 
                else if (fanOffTime > 0) {
                    PORTB = SetBit(PORTB, 2, 0);
                    fanOffTime--;
                }
                else if (dutyCycle == 0) {
                    PORTB = SetBit(PORTB, 2, 0);
                }
            }
            else {
                threshTemp = 90;
            }
            break;
        default:
            break;
    }
    return state;
}
/*
Tick function for the LCD screen.
*/
int LCDScreen_Tick(int state) {
    static unsigned char lastButtonState = 0;
    unsigned char buttonState = !(PINC & (1 << PC2));
    static unsigned char prevUpState = 0;
    static unsigned char prevDownState = 0;
    unsigned char upState = (ADC_read(1) > 900) ? 1 : 0;
    unsigned char downState = (ADC_read(1) < 100) ? 1 : 0;
    unsigned char upChanged = (upState != prevUpState);
    unsigned char downChanged = (downState != prevDownState);
    switch(state) { // State Transitions
        case LCD_SCREEN:
            state = LCD_SCREEN;
            break;
        default:
            state = LCD_SCREEN;
            break;
    }
    switch(state) { // State Actions
        case LCD_SCREEN:
            if (buttonState && !lastButtonState) {
                mode = !mode;
                modeDisplayed = 0;
                lcd_clear();
            }
            lastButtonState = buttonState;
            if (mode == 1) {
                if (upChanged && upState) {
                    threshTemp += 1;
                }
                if (downChanged && downState) {
                    threshTemp -= 1;
                }
            }
            if (!modeDisplayed) {
                if (mode == 0) {
                    thermistorCycleActivate = 0;
                    if (dutyCycleActivate == 1) {
                        lcd_goto_xy(1, 5);
                        fanRunTimeString[3] = '\0';
                        if (dutyCycle >= 10) {
                            fanRunTimeString[3] = '%';
                            fanRunTimeString[2] = '0';
                            fanRunTimeString[1] = '0';
                            fanRunTimeString[0] = '0' + (dutyCycle / 10);
                        } 
                        else {
                            fanRunTimeString[3] = ' ';
                            fanRunTimeString[2] = '%';
                            fanRunTimeString[1] = '0';
                            fanRunTimeString[0] = '0' + (dutyCycle % 10);
                        }
                        lcd_write_str(fanRunTimeString);
                    }
                    else {
                        lcd_write_str((char*)str1);
                        fanRunTimeString[3] = '\0';
                        if (dutyCycle >= 10) {
                            fanRunTimeString[3] = '%';
                            fanRunTimeString[2] = '0';
                            fanRunTimeString[1] = '0';
                            fanRunTimeString[0] = '0' + (dutyCycle / 10);
                        } 
                        else {
                            fanRunTimeString[2] = '%';
                            fanRunTimeString[1] = '0';
                            fanRunTimeString[0] = '0' + (dutyCycle % 10);
                        }
                        lcd_goto_xy(1, 0);
                        lcd_write_str((char*)str3);
                        lcd_write_str(fanRunTimeString);
                    }
                }
                else {
                    dutyCycleActivate = 0;
                    if (thermistorCycleActivate) {
                        lcd_goto_xy(1, 5);
                        tempStr[0] = '0' + ((char)tempF / 10);
                        tempStr[1] = '0' + ((char)tempF % 10);
                        tempStr[2] = 'F';
                        tempStr[3] = '\0';
                        lcd_write_str(tempStr);
                        lcd_goto_xy(1, 12);
                        threshStr[0] = '0' + ((char)threshTemp / 10);
                        threshStr[1] = '0' + ((char)threshTemp % 10);
                        threshStr[2] = 'F';
                        threshStr[3] = '\0';
                        lcd_write_str(threshStr);
                    }
                    else {
                        lcd_write_str((char*) str2);
                        lcd_goto_xy(1, 0);
                        lcd_write_str((char*) str4);
                        lcd_goto_xy(1, 5);
                        char tempStr[6];
                        tempStr[0] = '0' + ((char)tempF / 10);
                        tempStr[1] = '0' + ((char)tempF % 10);
                        tempStr[2] = 'F';
                        tempStr[3] = '\0';
                        lcd_write_str(tempStr);
                        lcd_goto_xy(1, 9);
                        lcd_write_str((char*) str5);
                        threshStr[0] = '0' + ((char)threshTemp / 10);
                        threshStr[1] = '0' + ((char)threshTemp % 10);
                        threshStr[2] = 'F';
                        threshStr[3] = '\0';
                        lcd_write_str(threshStr);
                    }
                }
                modeDisplayed = 1;
            }
            break;
        default:
            break;
    }
    prevUpState = upState;
    prevDownState = downState;
    return state;
}