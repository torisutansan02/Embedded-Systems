#ifndef ISR_H
#define ISR_H

#include <avr/interrupt.h>

typedef struct _task {
    signed char state;
    unsigned long period;
    unsigned long elapsedTime;
    int (*TickFct)(int);
} task;

#define NUM_TASKS 5

extern task tasks[NUM_TASKS];
extern volatile unsigned char TimerFlag;

void TimerISR(void);
void TimerSet(unsigned long M);
void TimerOn();
void TimerOff();
unsigned long int findGCD(unsigned long int a, unsigned long int b);

#endif // ISR_H
