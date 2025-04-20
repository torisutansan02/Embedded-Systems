#include "game/Sound.h"

// Note key (i), note value (i + 1)
const int melody[] = {
    NOTE_B4, 16, 
    NOTE_B5, 16, 
    NOTE_FS5, 16, 
    NOTE_DS5, 16,
    NOTE_B5, 32, 
    NOTE_FS5, -16, 
    NOTE_DS5, 8, 
    NOTE_C5, 16,
    NOTE_C6, 16, 
    NOTE_G6, 16, 
    NOTE_E6, 16, 
    NOTE_C6, 32, 
    NOTE_G6, -16, 
    NOTE_E6, 8,
    NOTE_B4, 16, 
    NOTE_B5, 16, 
    NOTE_FS5, 16, 
    NOTE_DS5, 16, 
    NOTE_B5, 32,
    NOTE_FS5, -16, 
    NOTE_DS5, 8, 
    NOTE_DS5, 32, 
    NOTE_E5, 32, 
    NOTE_F5, 32,
    NOTE_F5, 32, 
    NOTE_FS5, 32, 
    NOTE_G5, 32, 
    NOTE_G5, 32, 
    NOTE_GS5, 32, 
    NOTE_A5, 16, 
    NOTE_B5, 8
};

// Note key (i), note value (i + 1)
const int pacmanPelletsMelody[] = {
    NOTE_G3, 16,
    NOTE_C4, 16
};

// Note key (i), note value (i + 1)
const int pacmanLosingMelody[] = {
    NOTE_C5, 8,
    NOTE_B4, 8,
    NOTE_AS4, 8,
    NOTE_A4, 8,
    NOTE_G4, 8,
    NOTE_FS4, 8,
    NOTE_F4, 8,
    NOTE_E4, 8,
    NOTE_DS4, 8,
    NOTE_D4, 8,
    NOTE_CS4, 8,
    NOTE_C4, 16,
    NOTE_C4, 32,
    REST,    8
};

const int notes = sizeof(melody) / sizeof(melody[0]) / 2;
const int losingNotes = sizeof(pacmanLosingMelody) / sizeof(pacmanLosingMelody[0]) / 2;
const int tempo = 120;
const int wholenote = (60000 * 4) / tempo;

void playNote(int note, int duration) {
    if (note == REST) {
        custom_delay_ms(duration);
        return;
    }

    uint16_t cycles = 1000000 / note;
    uint16_t iterations = duration * 1000 / cycles;

    while (iterations--) {
        buzzer_on();
        custom_delay_us(cycles / 2);
        buzzer_off();
        custom_delay_us(cycles / 2);
    }
}

void playMelody(const int* sequence, int length) {
    for (int i = 0; i < length * 2; i += 2) {
        int duration = wholenote / abs(sequence[i + 1]);
        if (sequence[i + 1] < 0) duration *= 3;
        playNote(sequence[i], duration);
        custom_delay_ms(duration / 2);
    }
}