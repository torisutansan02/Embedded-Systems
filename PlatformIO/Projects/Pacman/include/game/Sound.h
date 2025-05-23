#ifndef SOUND_H
#define SOUND_H

#include "utils/DELAY.h"
#include "utils/BUZZER.h"

#include <stdlib.h>

constexpr int NOTE_B0  = 31;
constexpr int NOTE_C1  = 33;
constexpr int NOTE_CS1 = 35;
constexpr int NOTE_D1  = 37;
constexpr int NOTE_DS1 = 39;
constexpr int NOTE_E1  = 41;
constexpr int NOTE_F1  = 44;
constexpr int NOTE_FS1 = 46;
constexpr int NOTE_G1  = 49;
constexpr int NOTE_GS1 = 52;
constexpr int NOTE_A1  = 55;
constexpr int NOTE_AS1 = 58;
constexpr int NOTE_B1  = 62;
constexpr int NOTE_C2  = 65;
constexpr int NOTE_CS2 = 69;
constexpr int NOTE_D2  = 73;
constexpr int NOTE_DS2 = 78;
constexpr int NOTE_E2  = 82;
constexpr int NOTE_F2  = 87;
constexpr int NOTE_FS2 = 93;
constexpr int NOTE_G2  = 98;
constexpr int NOTE_GS2 = 104;
constexpr int NOTE_A2  = 110;
constexpr int NOTE_AS2 = 117;
constexpr int NOTE_B2  = 123;
constexpr int NOTE_C3  = 131;
constexpr int NOTE_CS3 = 139;
constexpr int NOTE_D3  = 147;
constexpr int NOTE_DS3 = 156;
constexpr int NOTE_E3  = 165;
constexpr int NOTE_F3  = 175;
constexpr int NOTE_FS3 = 185;
constexpr int NOTE_G3  = 196;
constexpr int NOTE_GS3 = 208;
constexpr int NOTE_A3  = 220;
constexpr int NOTE_AS3 = 233;
constexpr int NOTE_B3  = 247;
constexpr int NOTE_C4  = 262;
constexpr int NOTE_CS4 = 277;
constexpr int NOTE_D4  = 294;
constexpr int NOTE_DS4 = 311;
constexpr int NOTE_E4  = 330;
constexpr int NOTE_F4  = 349;
constexpr int NOTE_FS4 = 370;
constexpr int NOTE_G4  = 392;
constexpr int NOTE_GS4 = 415;
constexpr int NOTE_A4  = 440;
constexpr int NOTE_AS4 = 466;
constexpr int NOTE_B4  = 494;
constexpr int NOTE_C5  = 523;
constexpr int NOTE_CS5 = 554;
constexpr int NOTE_D5  = 587;
constexpr int NOTE_DS5 = 622;
constexpr int NOTE_E5  = 659;
constexpr int NOTE_F5  = 698;
constexpr int NOTE_FS5 = 740;
constexpr int NOTE_G5  = 784;
constexpr int NOTE_GS5 = 831;
constexpr int NOTE_A5  = 880;
constexpr int NOTE_AS5 = 932;
constexpr int NOTE_B5  = 988;
constexpr int NOTE_C6  = 1047;
constexpr int NOTE_CS6 = 1109;
constexpr int NOTE_D6  = 1175;
constexpr int NOTE_DS6 = 1245;
constexpr int NOTE_E6  = 1319;
constexpr int NOTE_F6  = 1397;
constexpr int NOTE_FS6 = 1480;
constexpr int NOTE_G6  = 1568;
constexpr int NOTE_GS6 = 1661;
constexpr int NOTE_A6  = 1760;
constexpr int NOTE_AS6 = 1865;
constexpr int NOTE_B6  = 1976;
constexpr int NOTE_C7  = 2093;
constexpr int NOTE_CS7 = 2217;
constexpr int NOTE_D7  = 2349;
constexpr int NOTE_DS7 = 2489;
constexpr int NOTE_E7  = 2637;
constexpr int NOTE_F7  = 2794;
constexpr int NOTE_FS7 = 2960;
constexpr int NOTE_G7  = 3136;
constexpr int NOTE_GS7 = 3322;
constexpr int NOTE_A7  = 3520;
constexpr int NOTE_AS7 = 3729;
constexpr int NOTE_B7  = 3951;
constexpr int NOTE_C8  = 4186;
constexpr int NOTE_CS8 = 4435;
constexpr int NOTE_D8  = 4699;
constexpr int NOTE_DS8 = 4978;
constexpr int REST     = 0;

extern const int melody[];
extern const int pacmanPelletsMelody[];
extern const int pacmanLosingMelody[];

extern const int notes;
extern const int losingNotes;
extern const int wholenote;

void playNote(int note, int duration);
void playMelody(const int* sequence, int length);

#endif
