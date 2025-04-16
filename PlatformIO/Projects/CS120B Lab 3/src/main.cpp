/*
Your Name and Email: Tristan Cai (tcai019@ucr.edu)
Discussion Section: 024
Assignment: Lab 3 Part 1
Exercise Description:

I acknowledge all content contained herein, excluding template or example code,
is my own original work.

Demo Link: https://www.youtube.com/watch?v=IX-rKOuV_Mc
*/
#include "Arduino.h"
#include "timerISR.h"

/*
Corresponds to digital or analog pins.
*/
int buttonPin = 5;
int photoresistorPin = A4;
int ledPinRed = 9;
int ledPinGreen = 10;
int ledPinBlue = 11;

int dark_threshold = 500;

/*
State machines for off, on.
*/
enum states {OFF, ON, TOGGLE} state;
bool isLightOn = false;
bool isDark = false;
unsigned long lastTriggerTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPinRed, OUTPUT);
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinBlue, OUTPUT);
  pinMode(photoresistorPin, INPUT);
  
  state = OFF;
  
  Serial.begin(9600);
  
  /*
  Set time to 1000, equal to 1 second for period.
  */
  TimerSet(1000);
  TimerOn();
}

/*
Ability to pick between 3 colors.
*/
void setColor(int red, int green, int blue) {
  analogWrite(ledPinRed, red);
  analogWrite(ledPinGreen, green);
  analogWrite(ledPinBlue, blue);
}

void Tick() {
  switch(state) {
    case OFF:
      if (digitalRead(buttonPin) == HIGH || isDark) {
        state = TOGGLE;
        Serial.println("Turn on");
        setColor(255, 255, 255);
        lastTriggerTime = millis();
      }
      break;
    case ON:
      if (digitalRead(buttonPin) == LOW || (!isDark && (millis() - lastTriggerTime >= 9000))) {
        state = TOGGLE;
        Serial.println("Turn off");
        setColor(0, 0, 0);
      }
      break;
    case TOGGLE:
      if (digitalRead(buttonPin) == HIGH) {
        state = isLightOn ? OFF : ON;
        isLightOn = !isLightOn;
      }
      break;
  }
}

void loop() {
  int photoresistorValue = analogRead(photoresistorPin);
  isDark = (photoresistorValue < dark_threshold);

  Tick();
  
  while(!TimerFlag){}
  TimerFlag = 0;
}
