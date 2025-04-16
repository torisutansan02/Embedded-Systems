#include <Arduino.h>

/*
The pins on the Arduino Uno
*/

const int ledPins[4] = {2, 3, 4, 5};
int inputVal = 0;

// // 7-segment display pins and implementation (change pin values if necessary):
// int a = 8;
// int b = 9;
// int c = 11;
// int d = 12;
// int e = 13;
// int f = 7;
// int g = 6;
// int dp = 10;
// int LEDS[7] {a, b, c, d, e, f, g};

// /*
// Binary values for 0 to 15.
// Used for displaying hex.
// */
// int nums[16][7] {
//   {1, 1, 1, 1, 1, 1, 0}, // 0
//   {0, 1, 1, 0, 0, 0, 0}, // 1
//   {1, 1, 0, 1, 1, 0, 1}, // 2
//   {1, 1, 1, 1, 0, 0, 1}, // 3
//   {0, 1, 1, 0, 0, 1, 1}, // 4
//   {1, 0, 1, 1, 0, 1, 1}, // 5
//   {1, 0, 1, 1, 1, 1, 1}, // 6
//   {1, 1, 1, 0, 0, 0, 0}, // 7
//   {1, 1, 1, 1, 1, 1, 1}, // 8
//   {1, 1, 1, 1, 0, 1, 1}, // 9
//   {1, 1, 1, 0, 1, 1, 1}, // A
//   {0, 0, 1, 1, 1, 1, 1}, // b
//   {1, 0, 0, 1, 1, 1, 0}, // C
//   {0, 1, 1, 1, 1, 0, 1}, // d
//   {1, 0, 0, 1, 1, 1, 1}, // E
//   {1, 0, 0, 0, 1, 1, 1} // F
// };

// /*
// Function used to display hex.
// This displays on 7-segment.
// */
// void outNum(int x) {
//   for (int i = 0; i < 7; i++) {
//     if (nums[x][i] == 1) {
//       digitalWrite(LEDS[i], 1);
//     }
//     else {
//       digitalWrite(LEDS[i], 0);
//     }
//   }
// }

void setup() 
{
  /*
  Configures the pin for input and output.
  */
  Serial.begin(9600);
  Serial.println("Enter a value within 0 and 15.");

  for (int i = 0; i < 4; i++) 
  {
    pinMode(ledPins[i], OUTPUT);
  }
  // for (int i = 0; i < 7; i++) 
  // {
  //   pinMode(LEDS[i], OUTPUT);
  // }
}

void loop() 
{
  if (Serial.available() > 0) 
  {
    inputVal = Serial.parseInt();
    /*
    Ensure an integer is closed within the range of 0 to 15.
    */
    if (inputVal >= 0 && inputVal <= 15) 
    {
      /*
      Writes values to the pin.
      Light on means True.
      Light off means False.
      */
      for (int i = 0; i < 4; i++) 
      {
        digitalWrite(ledPins[i], (inputVal >> i) & 1);
      }

      /*
      Use outNum function to display hexadecimal code.
      */

      // outNum(inputVal); // PART 3

      Serial.print("Input value is ");
      Serial.println(inputVal);
      Serial.print("Binary value is ");

      /*
      Print binary value
      */
      for (int i = 3; i >= 0; i--) {
        Serial.print((inputVal >> i) & 1);
      }
      Serial.println();
      
      /*
      Prompt user to input bit value.
      */
      Serial.println("Enter a bit from 0 to 3.");
      
      /*
      Input selectedBit value. PARTS 2 AND 3.
      */
      while (!Serial.available());
      int selectedBit = Serial.parseInt();
      
      if (selectedBit >= 0 && selectedBit <= 3) 
      {
        Serial.print("Bit value is ");
        Serial.println(selectedBit);
        /*
        Bit must be within 0 and 3.
        Turn off all LEDs except for masking + shifting.
        */
        for (int i = 0; i < 4; i++) 
        {
          digitalWrite(ledPins[i], LOW);
        }

        /*
        Mask and shift to the rightmost LED.
        */
        if ((inputVal >> selectedBit) & 1) 
        {
          for (int i = selectedBit; i >= 0; i--) 
          {
            digitalWrite(ledPins[i], HIGH);
            /*
            Display the binary value on 7-segment using outNum.
            */
      //       if (i == 3) 
      //       {
      //         outNum(8);
      //       }
      //       else if (i == 2) 
      //       {
      //         outNum(4);
      //       }
      //       else if (i == 1) 
      //       {
      //         outNum(2);
      //       }
      //       else if (i == 0) 
      //       {
      //         outNum(1);
      //       }
            delay(300);
            digitalWrite(ledPins[i + 1], LOW);
          }
          /*
          Shift inputVal.
          */
          inputVal >>= selectedBit;
        }
        else 
        {
          /*
          If bit value is 0, display 0 on 7-segment.
          */
          // outNum(0);
        }
        Serial.println("Enter a value within 0 and 15.");
      } 
      else 
      {
        Serial.println("Invalid. Please select a bit from 0 to 3.");
      }
    } 
    else 
    {
      /*
      User does not enter a value within 0 to 15.
      */
      Serial.println("Invalid input. Must be between 0 and 15.");
      Serial.println("Enter a value within 0 and 15.");
    }
  }
}
