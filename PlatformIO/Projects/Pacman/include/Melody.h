int melody[] = {
    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16,
    NOTE_B5, 32, NOTE_FS5, -16, NOTE_DS5, 8, NOTE_C5, 16,
    NOTE_C6, 16, NOTE_G6, 16, NOTE_E6, 16, NOTE_C6, 32, NOTE_G6, -16, NOTE_E6, 8,

    NOTE_B4, 16, NOTE_B5, 16, NOTE_FS5, 16, NOTE_DS5, 16, NOTE_B5, 32,
    NOTE_FS5, -16, NOTE_DS5, 8, NOTE_DS5, 32, NOTE_E5, 32, NOTE_F5, 32,
    NOTE_F5, 32, NOTE_FS5, 32, NOTE_G5, 32, NOTE_G5, 32, NOTE_GS5, 32, NOTE_A5, 16, NOTE_B5, 8
};

int pacmanPelletsMelody[] = {
    NOTE_G5, 50, NOTE_C6, 50, NOTE_E6, 50, NOTE_G6, 50
};

int pacmanLosingMelody[] = {
    NOTE_AS3, 16,    // A#3 for 4 beats (whole note)
    NOTE_F3, 16,     // F3 for 4 beats (whole note)
    NOTE_DS3, 16,    // D#3 for 4 beats (whole note)
    NOTE_D3, 32,     // D3 for 2 beats (half note)
    
    NOTE_AS3, 16,    // A#3 for 4 beats (whole note)
    NOTE_F3, 16,     // F3 for 4 beats (whole note)
    NOTE_DS3, 16,    // D#3 for 4 beats (whole note)
    NOTE_D3, 32,     // D3 for 2 beats (half note)
    
    NOTE_AS3, 16,    // A#3 for 4 beats (whole note)
    NOTE_F3, 16,     // F3 for 4 beats (whole note)
    NOTE_DS3, 16,    // D#3 for 4 beats (whole note)
    NOTE_D3, 32,     // D3 for 2 beats (half note)
    
    NOTE_G3, 16,     // G3 for 4 beats (whole note)
    NOTE_E3, 16,     // E3 for 4 beats (whole note)
    NOTE_C3, 64     // C3 for 1 beat (quarter note)
};

int notes = sizeof(melody) / sizeof(melody[0]) / 2;
int losingNotes = sizeof(pacmanLosingMelody) / sizeof(pacmanLosingMelody[0]) / 2;
int tempo = 120;
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;