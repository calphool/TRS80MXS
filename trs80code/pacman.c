/* *******************************************************************************************************************************
   | PAC-MAN-like game for TRS-80 Model 1 with TRS80MXS graphics/sound/joystick board                                            |
   | Copyright 2020 JOSEPH ROUNCEVILLE                                                                                           |
   |                                                                                                                             |
   | Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation  |
   | files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,  |
   | modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the       |
   | Software is furnished to do so, subject to the following conditions:                                                        |
   |                                                                                                                             |
   | The above copyright notice and this permission notice shall be included in all copies or substantial portions of the        |
   | Software.                                                                                                                   |
   |                                                                                                                             |
   | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE        |
   | WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR       |
   | COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, |
   | ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                       |
   *******************************************************************************************************************************
*/

// Uses z88dk compiler
// sample compilation command:
// zcc  +trs80 -O3 -lndos -lm -create-app --list -subtype=disk pacman.c
// or
// gcc-9 pacman.c -o pacman -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -lfluidsynth
//
// Note: gcc requires that you've installed fluidsynth and SDL2 on your machine (use brew install fluidsynth and brew install SDL on a Mac)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define byte unsigned char

#ifdef __APPLE__
  // compiled under GCC-9 rather than ZCC
  #define GCC_COMPILED
#endif

#ifdef GCC_COMPILED
    #include <SDL2/SDL.h>
    #include <fluidsynth.h>
    #include <time.h>
    #define SCREEN_WIDTH 512
    #define SCREEN_HEIGHT 384
   fluid_settings_t *settings;
   fluid_synth_t *synth = NULL;
   fluid_audio_driver_t *adriver = NULL;
#endif

// port addresses
#define LEFT_POS 0x00
#define RIGHT_POS 0x01

// colors
#define TRANSPARENT 0
#define BLACK 1
#define MEDIUMGREEN 2
#define LIGHTGREEN 3
#define DARKBLUE 4
#define LIGHTBLUE 5
#define DARKRED 6
#define CYAN 7
#define MEDIUMRED 8
#define LIGHTRED 9
#define DARKYELLOW 10
#define LIGHTYELLOW 11
#define DARKGREEN 12
#define MAGENTA 13
#define GRAY 14
#define WHITE 15


// joystick positions
// screwy assignments without ALPHASTICK adapter
/*
#define J_N 239
#define J_NE 111
#define J_E 127
#define J_SE 95
#define J_S 223
#define J_SW 159
#define J_W 191
#define J_NW 175
#define J_BUTTON 247
*/

// ALPHASTICK assignments
#define J_N 254
#define J_NE 246
#define J_E 247
#define J_SE 245
#define J_S 253
#define J_SW 249
#define J_W 251
#define J_NW 250
#define J_BUTTON 239


#define PATT_PACMAN_E1 0
#define PATT_PACMAN_E2 1
#define PATT_PACMAN_E3 2
#define PATT_PACMAN_N1 3
#define PATT_PACMAN_N2 4
#define PATT_PACMAN_N3 5
#define PATT_PACMAN_W1 6
#define PATT_PACMAN_W2 7
#define PATT_PACMAN_W3 8
#define PATT_PACMAN_S1 9
#define PATT_PACMAN_S2 10
#define PATT_PACMAN_S3 11
#define PATT_NORMAL_GHOST_1 12
#define PATT_NORMAL_GHOST_2 13
#define PATT_NORMAL_GHOST_EYES_N 14
#define PATT_NORMAL_GHOST_EYES_S 15
#define PATT_NORMAL_GHOST_EYES_E 16
#define PATT_NORMAL_GHOST_EYES_W 17
#define PATT_SCARED_GHOST 18
#define PATT_PACMAN_DYING_START 19
#define PATT_PACMAN_DYING_1 20
#define PATT_PACMAN_DYING_2 21
#define PATT_PACMAN_DYING_3 22
#define PATT_PACMAN_DYING_4 23
#define PATT_PACMAN_DYING_5 24
#define PATT_PACMAN_DYING_6 25
#define PATT_PACMAN_DYING_7 26
#define PATT_PACMAN_DYING_8 27
#define PATT_PACMAN_DYING_9 28
#define PATT_PACMAN_DYING_END 29
#define PATT_HAT 30
#define PATT_200 31
#define PATT_400 32
#define PATT_800 33
#define PATT_1600 34
#define PATT_QUESTIONMARKS 35

#define PATT_CHERRY 36
#define PATT_STRAWBERRY 37
#define PATT_APPLE 38
#define PATT_PEACH 39
#define PATT_MELON 40
#define PATT_GALAXIAN 41
#define PATT_BELL 42
#define PATT_KEY 43

#define ENERGIZER_RIGHT_CHAR 128
#define ENERGIZER_LEFT_CHAR 129

#define FLAG_CHAR 136


// sprite identifiers
#define PACMAN_SPRITENUM 0

#define HAT_SPRITENUM 1

#define RED_GHOST_SPRITENUM 2
#define CYAN_GHOST_SPRITENUM 3
#define PINK_GHOST_SPRITENUM 4
#define BROWN_GHOST_SPRITENUM 5

#define RED_GHOST_EYES_SPRITENUM 6
#define CYAN_GHOST_EYES_SPRITENUM 7
#define PINK_GHOST_EYES_SPRITENUM 8
#define BROWN_GHOST_EYES_SPRITENUM 9

#define POP_SCORE_SPRITENUM 10

#define FRUIT_SPRITENUM 11

#define MAX_SPRITENUM 11

#define EAST_PACMAN_PAT_OFFSET 0
#define NORTH_PACMAN_PAT_OFFSET 3 
#define WEST_PACMAN_PAT_OFFSET 6
#define SOUTH_PACMAN_PAT_OFFSET 9


#define PACMAN_HOME_X 120
#define PACMAN_HOME_Y 128


#define EIGHTHNOTE 1000
#define SIXTEENTHNOTE 500
#define THIRTYSECONDNOTE 250
#define SIXTYFOURTHNOTE 125
#define ONETWENTYEIGHTHNOTE 63
#define TWOFIFTYSIXTHNOTE 32


/* 0x00 = JOY 1 when IN */
/* 0x01 = JOY 2 when IN */

/* 0x82 = SOUND 1 when OUT */
/* 0x83 = SOUND 2 when OUT */
/* 0x84 = SOUND 3 when OUT */
/* 0x85 = SOUND 4 when OUT */
#ifndef GCC_COMPILED
__sfr __at 0x00 PORTX00;
__sfr __at 0x01 PORTX01;
__sfr __at 0x80 PORTX80;
__sfr __at 0x81 PORTX81;
__sfr __at 0x82 PORTX82;
__sfr __at 0x83 PORTX83;
__sfr __at 0x84 PORTX84;
__sfr __at 0x85 PORTX85;

    // note offsets
    #define BANK1_C9 14
    #define BANK1_B8 15
    #define BANK1_AS8 15
    #define BANK1_A8 16
    #define BANK1_GS8 17
    #define BANK1_G8 18
    #define BANK1_FS8 19
    #define BANK1_F8 20
    #define BANK1_E8 22
    #define BANK1_DS8 23
    #define BANK1_D8 24
    #define BANK1_CS8 25
    #define BANK1_C8 27
    #define BANK1_B7 29
    #define BANK1_AS7 30
    #define BANK1_A7 32
    #define BANK1_GS7 34
    #define BANK1_G7 36
    #define BANK1_FS7 38
    #define BANK1_F7 40
    #define BANK1_E7 43
    #define BANK1_DS7 45
    #define BANK1_D7 48
    #define BANK1_CS7 50
    #define BANK1_C7 54
    #define BANK1_B6 57
    #define BANK1_AS6 60
    #define BANK1_A6 64
    #define BANK1_GS6 68
    #define BANK1_G6 72
    #define BANK1_FS6 76
    #define BANK1_F6 80
    #define BANK1_E6 85
    #define BANK1_DS6 90
    #define BANK1_D6 96
    #define BANK1_CS6 101
    #define BANK1_C6 107
    #define BANK1_B5 114
    #define BANK1_AS5 120
    #define BANK1_A5 127
    #define BANK1_GS5 135
    #define BANK1_G5 143
    #define BANK1_FS5 151
    #define BANK1_F5 160
    #define BANK1_E5 170
    #define BANK1_DS5 180
    #define BANK1_D5 191
    #define BANK1_CS5 202
    #define BANK1_C5 214
    #define BANK1_B4 227
    #define BANK1_AS4 240
    #define BANK1_A4 254
    #define BANK1_GS4 269
    #define BANK1_G4 285
    #define BANK1_FS4 302
    #define BANK1_F4 320
    #define BANK1_E4 339
    #define BANK1_DS4 360
    #define BANK1_D4 381
    #define BANK1_CS4 404
    #define BANK1_C4 428
    #define BANK1_B3 453
    #define BANK1_AS3 480
    #define BANK1_A3 508
    #define BANK1_GS3 538
    #define BANK1_G3 570
    #define BANK1_FS3 604
    #define BANK1_F3 640
    #define BANK1_E3 678
    #define BANK1_DS3 720
    #define BANK1_D3 762
    #define BANK1_CS3 808
    #define BANK1_C3 856
    #define BANK1_B2 906
    #define BANK1_AS2 960
    #define BANK1_A2 1017

    // bank 2 is for 0x84 and 0x85 chips (2 octaves lower than 0x82 and 0x83)
    #define BANK2_C7 14
    #define BANK2_B6 15
    #define BANK2_AS6 15
    #define BANK2_A6 16
    #define BANK2_GS6 17
    #define BANK2_G6 18
    #define BANK2_FS6 19
    #define BANK2_F6 20
    #define BANK2_E6 22
    #define BANK2_DS6 23
    #define BANK2_D6 24
    #define BANK2_CS6 25
    #define BANK2_C6 27
    #define BANK2_B5 29
    #define BANK2_AS5 30
    #define BANK2_A5 32
    #define BANK2_GS5 34
    #define BANK2_G5 36
    #define BANK2_FS5 38
    #define BANK2_F5 40
    #define BANK2_E5 43
    #define BANK2_DS5 45
    #define BANK2_D5 48
    #define BANK2_CS5 50
    #define BANK2_C5 54
    #define BANK2_B4 57
    #define BANK2_AS4 60
    #define BANK2_A4 64
    #define BANK2_GS4 68
    #define BANK2_G4 72
    #define BANK2_FS4 76
    #define BANK2_F4 80
    #define BANK2_E4 85
    #define BANK2_DS4 90
    #define BANK2_D4 96
    #define BANK2_CS4 101
    #define BANK2_C4 107
    #define BANK2_B3 114
    #define BANK2_AS3 120
    #define BANK2_A3 127
    #define BANK2_GS3 135
    #define BANK2_G3 143
    #define BANK2_FS3 151
    #define BANK2_F3 160
    #define BANK2_E3 170
    #define BANK2_DS3 180
    #define BANK2_D3 191
    #define BANK2_CS3 202
    #define BANK2_C3 214
    #define BANK2_B2 227
    #define BANK2_AS2 240
    #define BANK2_A2 254
    #define BANK2_GS2 269
    #define BANK2_G2 285
    #define BANK2_FS2 302
    #define BANK2_F2 320
    #define BANK2_E2 339
    #define BANK2_DS2 360
    #define BANK2_D2 381
    #define BANK2_CS2 404
    #define BANK2_C2 428
    #define BANK2_B1 453
    #define BANK2_AS1 480
    #define BANK2_A1 508
    #define BANK2_GS1 538
    #define BANK2_G1 570
    #define BANK2_FS1 604
    #define BANK2_F1 640
    #define BANK2_E1 678
    #define BANK2_DS1 720
    #define BANK2_D1 762
    #define BANK2_CS1 808
    #define BANK2_C1 856
    #define BANK2_B0 906
    #define BANK2_AS0 960
    #define BANK2_A0 1017
#else
   char screen_buffer[65535];
   char VDPRegisters[8];
   SDL_Window* window = NULL;
   SDL_Surface* screenSurface = NULL;
   SDL_Renderer* renderer = NULL;
   SDL_Texture * texture = NULL;
   Uint32 * pixels = NULL;

// note offsets
    #define BANK1_C9 120
    #define BANK1_B8 119
    #define BANK1_AS8 118
    #define BANK1_A8 117
    #define BANK1_GS8 116
    #define BANK1_G8 115
    #define BANK1_FS8 114
    #define BANK1_F8 113
    #define BANK1_E8 112
    #define BANK1_DS8 111
    #define BANK1_D8 110
    #define BANK1_CS8 109
    #define BANK1_C8 108
    #define BANK1_B7 107
    #define BANK1_AS7 106
    #define BANK1_A7 105
    #define BANK1_GS7 104
    #define BANK1_G7 103
    #define BANK1_FS7 102
    #define BANK1_F7 101
    #define BANK1_E7 100
    #define BANK1_DS7 99
    #define BANK1_D7 98
    #define BANK1_CS7 97
    #define BANK1_C7 96
    #define BANK1_B6 95
    #define BANK1_AS6 94
    #define BANK1_A6 93
    #define BANK1_GS6 92
    #define BANK1_G6 91
    #define BANK1_FS6 90
    #define BANK1_F6 89
    #define BANK1_E6 88
    #define BANK1_DS6 87
    #define BANK1_D6 86
    #define BANK1_CS6 85
    #define BANK1_C6 84
    #define BANK1_B5 83
    #define BANK1_AS5 82
    #define BANK1_A5 81
    #define BANK1_GS5 80
    #define BANK1_G5 79
    #define BANK1_FS5 78
    #define BANK1_F5 77
    #define BANK1_E5 76
    #define BANK1_DS5 75
    #define BANK1_D5 74
    #define BANK1_CS5 73
    #define BANK1_C5 72
    #define BANK1_B4 71
    #define BANK1_AS4 70
    #define BANK1_A4 69
    #define BANK1_GS4 68
    #define BANK1_G4 67
    #define BANK1_FS4 66
    #define BANK1_F4 65
    #define BANK1_E4 64
    #define BANK1_DS4 63
    #define BANK1_D4 62
    #define BANK1_CS4 61
    #define BANK1_C4 60
    #define BANK1_B3 59
    #define BANK1_AS3 58
    #define BANK1_A3 57
    #define BANK1_GS3 56
    #define BANK1_G3 55
    #define BANK1_FS3 54
    #define BANK1_F3 53
    #define BANK1_E3 52
    #define BANK1_DS3 51
    #define BANK1_D3 50
    #define BANK1_CS3 49
    #define BANK1_C3 48
    #define BANK1_B2 47
    #define BANK1_AS2 46
    #define BANK1_A2 45

    // bank 2 is for 0x84 and 0x85 chips (2 octaves lower than 0x82 and 0x83)
    #define BANK2_C7 96
    #define BANK2_B6 95
    #define BANK2_AS6 94
    #define BANK2_A6 93
    #define BANK2_GS6 92
    #define BANK2_G6 91
    #define BANK2_FS6 90
    #define BANK2_F6 89
    #define BANK2_E6 88
    #define BANK2_DS6 87
    #define BANK2_D6 86
    #define BANK2_CS6 85
    #define BANK2_C6 84
    #define BANK2_B5 83
    #define BANK2_AS5 82
    #define BANK2_A5 81
    #define BANK2_GS5 80
    #define BANK2_G5 79
    #define BANK2_FS5 78
    #define BANK2_F5 77
    #define BANK2_E5 76
    #define BANK2_DS5 75
    #define BANK2_D5 74
    #define BANK2_CS5 73
    #define BANK2_C5 72
    #define BANK2_B4 71
    #define BANK2_AS4 70
    #define BANK2_A4 69
    #define BANK2_GS4 68
    #define BANK2_G4 67
    #define BANK2_FS4 66
    #define BANK2_F4 65
    #define BANK2_E4 64
    #define BANK2_DS4 63
    #define BANK2_D4 62
    #define BANK2_CS4 61
    #define BANK2_C4 60
    #define BANK2_B3 59
    #define BANK2_AS3 58
    #define BANK2_A3 57
    #define BANK2_GS3 56
    #define BANK2_G3 55
    #define BANK2_FS3 54
    #define BANK2_F3 53
    #define BANK2_E3 52
    #define BANK2_DS3 51
    #define BANK2_D3 50
    #define BANK2_CS3 49
    #define BANK2_C3 48
    #define BANK2_B2 47
    #define BANK2_AS2 46
    #define BANK2_A2 45
    #define BANK2_GS2 44
    #define BANK2_G2 43
    #define BANK2_FS2 42
    #define BANK2_F2 41
    #define BANK2_E2 40
    #define BANK2_DS2 39
    #define BANK2_D2 38
    #define BANK2_CS2 37
    #define BANK2_C2 36
    #define BANK2_B1 35
    #define BANK2_AS1 34
    #define BANK2_A1 33
    #define BANK2_GS1 32
    #define BANK2_G1 31
    #define BANK2_FS1 30
    #define BANK2_F1 29
    #define BANK2_E1 28
    #define BANK2_DS1 27
    #define BANK2_D1 26
    #define BANK2_CS1 25
    #define BANK2_C1 24
    #define BANK2_B0 23
    #define BANK2_AS0 22
    #define BANK2_A0 21
#endif

// graphicsSetupStruct constants
#define GRAPHICSMODE1 0
#define GRAPHICSMODE2 1
#define MULTICOLORMODE 2
#define TEXTMODE 3

// booleans
#define TRUE 1
#define FALSE 0

// ordinals
#define NORTH 0
#define SOUTH 1
#define WEST 2
#define EAST 3

// chip identifiers
#define AUDIOCHIP0 0x82
#define AUDIOCHIP1 0x83
#define AUDIOCHIP2 0x84
#define AUDIOCHIP3 0x85

// video setup structure
typedef struct {
    byte graphicsMode;
    byte externalVideoEnabled;
    byte interruptsEnabled;
    byte sprites16x16Enabled;
    byte spritesMagnified;
    byte textForegroundColor;
    byte backgroundColor;
    unsigned int NameTableAddr;
    unsigned int ColorTableAddr;
    unsigned int PatternTableAddr;
    unsigned int spriteAttrStructTableAddr;
    unsigned int SpritePatternTableAddr;
} graphicsSetupStruct;


//sprite attribute structure
typedef struct {
  int x;
  int y;
  int color;
  int xdir;
  int ydir;
  int patt;
  int prevdir;
  int isEyes;
} spriteAttrStruct;

char buf[33];                               // work buffer
int gameLoopRunning = FALSE;                // main game loop control
unsigned int pacmanAnimationPosition = 0;   // animation position
int pacmanAnimationDirection = 1;           // 1, -1 counter direction
graphicsSetupStruct g;                      // graphics mode struct
spriteAttrStruct spriteAttrArray[32];       // sprite struct array
unsigned long score = 0;                    // score field
unsigned int dotCtr = 0;                    // counter to decide when we're done with a screen
unsigned int energizerCtr = 0;              // count down counter when an energizer is eaten
unsigned int ghostCtr = 0;                  // score counter that adds up with each ghost being eaten
int lives = 3;                              // pacman lives
unsigned int gameCtr = 0;                   // number that counts up each frame as game plays
int GhostWithHat = 255;                     // which ghost has the hat?
unsigned int levelCtr = 1;                  // what level am I on?
int popScoreGameCtr = 0;                    // gameCtr value when ghost was eaten (used to remove popup score)
int fruitEatenOnThisLevel = 0;              // flag that flips if a fruit has been eaten

int bgMusicDirection = 0;




#define getRand256() ((unsigned char)(rand() % 256))

// circular buffer used in audio management
typedef struct {
    int * buffer;
    int head;
    int tail;
    int maxlen;
} circ_bbuf_t;

// circular buffer defining macro
#define CIRC_BBUF_DEF(x,y)                        \
    int x##_data_space[y];                        \
    circ_bbuf_t x = {                             \
        x##_data_space,                           \
        0,                                        \
        0,                                        \
        y};                                                                               

CIRC_BBUF_DEF(audio_circ_buffer_x83_0,32)


int circ_bbuf_pop(circ_bbuf_t *c, int *data)  // used to pop a value off of a circular buffer
{                                                 
    int next;                                     
    if (c->head == c->tail)                       
        return -1;
    next = c->tail + 1;                           
    if(next >= c->maxlen)
        next = 0;
    *data = c->buffer[c->tail];
    c->tail = next;             
    return 0;  
}
int circ_bbuf_push(circ_bbuf_t *c, int data)  // used to push a value onto a circular buffer
{
    int next;
    next = c->head + 1;
    if (next >= c->maxlen)
        next = 0;
    if (next == c->tail)
        return -1;
    c->buffer[c->head] = data;
    c->head = next;
    return 0;
}
int circ_bbuf_empty(circ_bbuf_t *c) {         // check if circular buffer is empty or not
    if(c->head == c->tail)
      return 1;

    return 0;
}



// audio pitch lookup table for pitches 0 - 1023  
// these bytes were generated through an algorithm that is necessary to feed the the right bits to the SN76489 chips
// there is some documentation at https://www.smspower.org/Development/SN76489
//
byte audioBytes[] = {
1,0,129,0,65,0,193,0,33,0,161,0,97,0,225,0,17,0,145,0,81,0,209,0,49,0,177,0,113,0,241,0,1,128,129,128,65,128,193,128,33,128,161,128,97,
128,225,128,17,128,145,128,81,128,209,128,49,128,177,128,113,128,241,128,1,64,129,64,65,64,193,64,33,64,161,64,97,64,225,64,17,64,145,64,
81,64,209,64,49,64,177,64,113,64,241,64,1,192,129,192,65,192,193,192,33,192,161,192,97,192,225,192,17,192,145,192,81,192,209,192,49,192,177,
192,113,192,241,192,1,32,129,32,65,32,193,32,33,32,161,32,97,32,225,32,17,32,145,32,81,32,209,32,49,32,177,32,113,32,241,32,1,160,129,160,65,
160,193,160,33,160,161,160,97,160,225,160,17,160,145,160,81,160,209,160,49,160,177,160,113,160,241,160,1,96,129,96,65,96,193,96,33,96,161,96,
97,96,225,96,17,96,145,96,81,96,209,96,49,96,177,96,113,96,241,96,1,224,129,224,65,224,193,224,33,224,161,224,97,224,225,224,17,224,145,224,81,
224,209,224,49,224,177,224,113,224,241,224,1,16,129,16,65,16,193,16,33,16,161,16,97,16,225,16,17,16,145,16,81,16,209,16,49,16,177,16,113,16,
241,16,1,144,129,144,65,144,193,144,33,144,161,144,97,144,225,144,17,144,145,144,81,144,209,144,49,144,177,144,113,144,241,144,1,80,129,80,65,
80,193,80,33,80,161,80,97,80,225,80,17,80,145,80,81,80,209,80,49,80,177,80,113,80,241,80,1,208,129,208,65,208,193,208,33,208,161,208,97,208,
225,208,17,208,145,208,81,208,209,208,49,208,177,208,113,208,241,208,1,48,129,48,65,48,193,48,33,48,161,48,97,48,225,48,17,48,145,48,81,48,
209,48,49,48,177,48,113,48,241,48,1,176,129,176,65,176,193,176,33,176,161,176,97,176,225,176,17,176,145,176,81,176,209,176,49,176,177,176,
113,176,241,176,1,112,129,112,65,112,193,112,33,112,161,112,97,112,225,112,17,112,145,112,81,112,209,112,49,112,177,112,113,112,241,112,1,
240,129,240,65,240,193,240,33,240,161,240,97,240,225,240,17,240,145,240,81,240,209,240,49,240,177,240,113,240,241,240,1,8,129,8,65,8,193,8,
33,8,161,8,97,8,225,8,17,8,145,8,81,8,209,8,49,8,177,8,113,8,241,8,1,136,129,136,65,136,193,136,33,136,161,136,97,136,225,136,17,136,145,
136,81,136,209,136,49,136,177,136,113,136,241,136,1,72,129,72,65,72,193,72,33,72,161,72,97,72,225,72,17,72,145,72,81,72,209,72,49,72,177,72,
113,72,241,72,1,200,129,200,65,200,193,200,33,200,161,200,97,200,225,200,17,200,145,200,81,200,209,200,49,200,177,200,113,200,241,200,1,40,
129,40,65,40,193,40,33,40,161,40,97,40,225,40,17,40,145,40,81,40,209,40,49,40,177,40,113,40,241,40,1,168,129,168,65,168,193,168,33,168,161,
168,97,168,225,168,17,168,145,168,81,168,209,168,49,168,177,168,113,168,241,168,1,104,129,104,65,104,193,104,33,104,161,104,97,104,225,104,
17,104,145,104,81,104,209,104,49,104,177,104,113,104,241,104,1,232,129,232,65,232,193,232,33,232,161,232,97,232,225,232,17,232,145,232,81,
232,209,232,49,232,177,232,113,232,241,232,1,24,129,24,65,24,193,24,33,24,161,24,97,24,225,24,17,24,145,24,81,24,209,24,49,24,177,24,113,24,
241,24,1,152,129,152,65,152,193,152,33,152,161,152,97,152,225,152,17,152,145,152,81,152,209,152,49,152,177,152,113,152,241,152,1,88,129,88,
65,88,193,88,33,88,161,88,97,88,225,88,17,88,145,88,81,88,209,88,49,88,177,88,113,88,241,88,1,216,129,216,65,216,193,216,33,216,161,216,97,
216,225,216,17,216,145,216,81,216,209,216,49,216,177,216,113,216,241,216,1,56,129,56,65,56,193,56,33,56,161,56,97,56,225,56,17,56,145,56,81,
56,209,56,49,56,177,56,113,56,241,56,1,184,129,184,65,184,193,184,33,184,161,184,97,184,225,184,17,184,145,184,81,184,209,184,49,184,177,184,
113,184,241,184,1,120,129,120,65,120,193,120,33,120,161,120,97,120,225,120,17,120,145,120,81,120,209,120,49,120,177,120,113,120,241,120,1,248,
129,248,65,248,193,248,33,248,161,248,97,248,225,248,17,248,145,248,81,248,209,248,49,248,177,248,113,248,241,248,1,4,129,4,65,4,193,4,33,4,
161,4,97,4,225,4,17,4,145,4,81,4,209,4,49,4,177,4,113,4,241,4,1,132,129,132,65,132,193,132,33,132,161,132,97,132,225,132,17,132,145,132,81,
132,209,132,49,132,177,132,113,132,241,132,1,68,129,68,65,68,193,68,33,68,161,68,97,68,225,68,17,68,145,68,81,68,209,68,49,68,177,68,113,68,
241,68,1,196,129,196,65,196,193,196,33,196,161,196,97,196,225,196,17,196,145,196,81,196,209,196,49,196,177,196,113,196,241,196,1,36,129,36,
65,36,193,36,33,36,161,36,97,36,225,36,17,36,145,36,81,36,209,36,49,36,177,36,113,36,241,36,1,164,129,164,65,164,193,164,33,164,161,164,97,
164,225,164,17,164,145,164,81,164,209,164,49,164,177,164,113,164,241,164,1,100,129,100,65,100,193,100,33,100,161,100,97,100,225,100,17,100,
145,100,81,100,209,100,49,100,177,100,113,100,241,100,1,228,129,228,65,228,193,228,33,228,161,228,97,228,225,228,17,228,145,228,81,228,209,
228,49,228,177,228,113,228,241,228,1,20,129,20,65,20,193,20,33,20,161,20,97,20,225,20,17,20,145,20,81,20,209,20,49,20,177,20,113,20,241,20,
1,148,129,148,65,148,193,148,33,148,161,148,97,148,225,148,17,148,145,148,81,148,209,148,49,148,177,148,113,148,241,148,1,84,129,84,65,84,
193,84,33,84,161,84,97,84,225,84,17,84,145,84,81,84,209,84,49,84,177,84,113,84,241,84,1,212,129,212,65,212,193,212,33,212,161,212,97,212,
225,212,17,212,145,212,81,212,209,212,49,212,177,212,113,212,241,212,1,52,129,52,65,52,193,52,33,52,161,52,97,52,225,52,17,52,145,52,81,52,
209,52,49,52,177,52,113,52,241,52,1,180,129,180,65,180,193,180,33,180,161,180,97,180,225,180,17,180,145,180,81,180,209,180,49,180,177,180,
113,180,241,180,1,116,129,116,65,116,193,116,33,116,161,116,97,116,225,116,17,116,145,116,81,116,209,116,49,116,177,116,113,116,241,116,1,
244,129,244,65,244,193,244,33,244,161,244,97,244,225,244,17,244,145,244,81,244,209,244,49,244,177,244,113,244,241,244,1,12,129,12,65,12,193,
12,33,12,161,12,97,12,225,12,17,12,145,12,81,12,209,12,49,12,177,12,113,12,241,12,1,140,129,140,65,140,193,140,33,140,161,140,97,140,225,140,
17,140,145,140,81,140,209,140,49,140,177,140,113,140,241,140,1,76,129,76,65,76,193,76,33,76,161,76,97,76,225,76,17,76,145,76,81,76,209,76,49,
76,177,76,113,76,241,76,1,204,129,204,65,204,193,204,33,204,161,204,97,204,225,204,17,204,145,204,81,204,209,204,49,204,177,204,113,204,241,
204,1,44,129,44,65,44,193,44,33,44,161,44,97,44,225,44,17,44,145,44,81,44,209,44,49,44,177,44,113,44,241,44,1,172,129,172,65,172,193,172,33,
172,161,172,97,172,225,172,17,172,145,172,81,172,209,172,49,172,177,172,113,172,241,172,1,108,129,108,65,108,193,108,33,108,161,108,97,108,
225,108,17,108,145,108,81,108,209,108,49,108,177,108,113,108,241,108,1,236,129,236,65,236,193,236,33,236,161,236,97,236,225,236,17,236,145,
236,81,236,209,236,49,236,177,236,113,236,241,236,1,28,129,28,65,28,193,28,33,28,161,28,97,28,225,28,17,28,145,28,81,28,209,28,49,28,177,28,
113,28,241,28,1,156,129,156,65,156,193,156,33,156,161,156,97,156,225,156,17,156,145,156,81,156,209,156,49,156,177,156,113,156,241,156,1,92,
129,92,65,92,193,92,33,92,161,92,97,92,225,92,17,92,145,92,81,92,209,92,49,92,177,92,113,92,241,92,1,220,129,220,65,220,193,220,33,220,161,
220,97,220,225,220,17,220,145,220,81,220,209,220,49,220,177,220,113,220,241,220,1,60,129,60,65,60,193,60,33,60,161,60,97,60,225,60,17,60,145,
60,81,60,209,60,49,60,177,60,113,60,241,60,1,188,129,188,65,188,193,188,33,188,161,188,97,188,225,188,17,188,145,188,81,188,209,188,49,188,
177,188,113,188,241,188,1,124,129,124,65,124,193,124,33,124,161,124,97,124,225,124,17,124,145,124,81,124,209,124,49,124,177,124,113,124,241,
124,1,252,129,252,65,252,193,252,33,252,161,252,97,252,225,252,17,252,145,252,81,252,209,252,49,252,177,252,113,252,241,252
};



/* *******************************************************************************************************************************
   | Set the volume of a given channel on a given port (chip 0x82 - 0x85)                                                        |
   *******************************************************************************************************************************
*/
void setVolume(byte chipID, byte voiceNum, byte bVolume) {
  #ifdef GCC_COMPILED
       static int midichan;
       midichan = (chipID - 0x82)*3 + voiceNum;
       fluid_synth_cc(synth, midichan, 7, (int) bVolume*8+7);
  #else 
      // precalculated bytes for the SN76489 chips (TRS80MXS has four of them)
      static byte bVolumeLookup[4][16] = {
            249,121,185,57,217,89,153,25,233,105,169,41,201,73,137,9,
            253,125,189,61,221,93,157,29,237,109,173,45,205,77,141,13,
            251,123,187,59,219,91,155,27,235,107,171,43,203,75,139,11,
            255,127,191,63,223,95,159,31,239,111,175,47,207,79,143,15
      };

        switch(chipID) {
          case 0x82:
              PORTX82 = bVolumeLookup[voiceNum][bVolume];
              break;
          case 0x83:
              PORTX83 = bVolumeLookup[voiceNum][bVolume];
              break;
          case 0x84:
              PORTX84 = bVolumeLookup[voiceNum][bVolume];
              break;      
          default:
              PORTX85 = bVolumeLookup[voiceNum][bVolume];
              break;
        }
  #endif
}


/* *******************************************************************************************************************************
   | Play a pitch in a given port (chip 0x82 - 0x85), on a given channel (3 per chip for tonal notes), at a given volume         |
   *******************************************************************************************************************************
*/
void play(byte chipID, byte voiceNum, int pitch, byte volume) {
    #ifdef GCC_COMPILED
        static int midichan;
        static int prevNoteForThisChannel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        midichan = (chipID - 0x82)*3 + voiceNum;

        fluid_synth_noteoff(synth, midichan, prevNoteForThisChannel[midichan]);
        fluid_synth_cc(synth, midichan, 7, (int) volume*8+7);
        fluid_synth_noteon(synth, midichan, pitch, (int)volume*8+7);
        prevNoteForThisChannel[midichan] = pitch;
    #else
        static byte leftsixbits;
        static byte b1;
        static int pdoub;

        pdoub = pitch << 1;

        setVolume(chipID, voiceNum, volume);

        b1 = *(audioBytes+pdoub);                    // offset into audioBytes buffer based on pitch value
        leftsixbits = *(audioBytes+pdoub+1);         // offset into audioBytes buffer + 1

        if(voiceNum == 1)                            // the audioBytes data has to be updated based on which voice you are playing
          b1 = b1 + 4;                               // the audioBytes data itself is only for voice 0
        else
          if(voiceNum == 2)
            b1 = b1 + 2;
          else
            if(voiceNum == 3)
              b1 = b1 + 6;

        switch(chipID) {
          case 0x82:
              PORTX82 = b1;
              PORTX82 = leftsixbits;
          break;

          case 0x83:
              PORTX83 = b1;
              PORTX83 = leftsixbits;
          break;

          case 0x84:
              PORTX84 = b1;
              PORTX84 = leftsixbits;
          break;

          default:
              PORTX85 = b1;
              PORTX85 = leftsixbits;
          break;
        }
    #endif
}


/* *******************************************************************************************************************************
   | Pull byte from VDP RAM on the TMS9118                                                                                       |
   *******************************************************************************************************************************
*/
byte getVDPRAM(unsigned int addr) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
#ifdef GCC_COMPILED
    return screen_buffer[addr];
#else
    static unsigned int addr_2;
    static unsigned int addr_1;
    
    addr_2 = (addr >> 8);
    addr_1 = addr - (addr_2 << 8);

    PORTX81 = (byte)addr_1;
    PORTX81 = (byte)addr_2;
    return PORTX80;
#endif
}

/* ******************************************************************************************************************************
   | Retrieve character at position (x,y) on the screen                                                                         |
   ******************************************************************************************************************************
*/
byte getCharFromNameTable(byte x, byte y)
{
    static unsigned int addr;
    addr = g.NameTableAddr;
    addr = addr + (y<<5) + x;

    return getVDPRAM(addr);
}

/* ******************************************************************************************************************************
   | do nothing X times, used for timing                                                                                        |
   ******************************************************************************************************************************
*/
void hold(unsigned int x) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
#ifdef GCC_COMPILED
    SDL_Delay(x >> 3);
#else
    for(unsigned int y = 0; y<x;y++) {
      #pragma asm
      nop
      #pragma endasm
    }
#endif
}


#ifdef GCC_COMPILED
/* *****************************************************************************************************************************
   | 32 bit memset                                                                                                             |
   *****************************************************************************************************************************
*/
void *memset32(void *m, uint32_t val, size_t count) {
    uint32_t *buf = m;
    while(count--) *buf++ = val;
    return m;
}

/* *****************************************************************************************************************************
   | given TMS9118 color, return ARGB value                                                                                    |
   *****************************************************************************************************************************
*/
Uint32 getARGBFromTMS9118Color(char cTMS9118Color) {
        switch(cTMS9118Color) {
          case 0: 
              return 0xff000000; // transparent
          case 1:
              return 0xff000000; // black
          case 2:
              return 0xff21c842; // green
          case 3:
              return 0xff5edc78; // light green
          case 4:
              return 0xff5455ed; // blue
          case 5:
              return 0xff7d76fc; // light blue 
          case 6:
              return 0xffd4524d; // dark red
          case 7:
              return 0xff42ebf5; // cyan 
          case 8:
              return 0xfffc5554; // red 
          case 9:
              return 0xffff7978; // light red
          case 10:
              return 0xffd4c154; // dark yellow 
          case 11:
              return 0xffe6ce80; // light yellow
          case 12:
              return 0xff21b03b; // dark green
          case 13:
              return 0xffc95bba; // megenta
          case 14:
              return 0xffcccccc; // gray
          default:
              return 0xffffffff; // white
        }
}

/* ******************************************************************************************************************************
   | Fill background with ARGB value                                                                                            |
   ******************************************************************************************************************************
*/
void paintBGColorOnSDLWindow() {
    memset32(pixels, getARGBFromTMS9118Color(VDPRegisters[7] & 0x0f), SCREEN_WIDTH*SCREEN_HEIGHT);
}

/* ******************************************************************************************************************************
   | Plot a pixel on the SDL window                                                                                             |
   ******************************************************************************************************************************
*/
void plotSDLPixel(int x, int y, Uint32 color) {    // x = 0 - 255, y = 0 - 191
    memset32(pixels + (x<<1) + y*SCREEN_WIDTH*2, color, 2); 
    memset32(pixels + (x<<1) + y*SCREEN_WIDTH*2 + SCREEN_WIDTH, color, 2);     
}

/* ******************************************************************************************************************************
   | get foreground ARGB color for a given character                                                                            |
   ******************************************************************************************************************************
*/
Uint32 getFGColorForChar(byte ch) 
{
    byte raw = getVDPRAM(g.ColorTableAddr + (ch >> 3));

    raw = raw >> 4; // fg color
    return getARGBFromTMS9118Color(raw);
}

/* ******************************************************************************************************************************
   | get background ARGB color for a given character                                                                            |
   ******************************************************************************************************************************
*/
Uint32 getBGColorForChar(byte ch) 
{
    byte raw = getVDPRAM(g.ColorTableAddr + (ch >> 3));

    raw = raw & 0x0f; // bg color
    return getARGBFromTMS9118Color(raw);
}

/* *****************************************************************************************************************************
   | plot character on SDL window                                                                                              |
   *****************************************************************************************************************************
*/
void plotCharOnSDLPixels(int xTMS9118, int yTMS9118, byte c) {
  unsigned int pattern_addr = (c<<3) + g.PatternTableAddr;
  Uint32 fgCol = getFGColorForChar(c);
  Uint32 bgCol = getBGColorForChar(c);

  for(int i=0;i<8;++i) {  
    c = screen_buffer[pattern_addr+i]; // now c = something like 0x55
    if(c >= 128) {
      plotSDLPixel((xTMS9118<<3), (yTMS9118<<3)+i,fgCol);
      c=c-128;
    }
    else 
      plotSDLPixel((xTMS9118<<3), (yTMS9118<<3)+i,bgCol);

    if(c >= 64) {
      plotSDLPixel((xTMS9118<<3)+1, (yTMS9118<<3)+i,fgCol);
      c=c-64;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+1, (yTMS9118<<3)+i,bgCol);

    if(c >= 32) {
      plotSDLPixel((xTMS9118<<3)+2, (yTMS9118<<3)+i,fgCol);
      c=c-32;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+2, (yTMS9118<<3)+i,bgCol);

    if(c >= 16) {
      plotSDLPixel((xTMS9118<<3)+3, (yTMS9118<<3)+i,fgCol);
      c=c-16;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+3, (yTMS9118<<3)+i,bgCol);

    if(c >= 8) {
      plotSDLPixel((xTMS9118<<3)+4, (yTMS9118<<3)+i,fgCol);
      c=c-8;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+4, (yTMS9118<<3)+i,bgCol);

    if(c >= 4) {
      plotSDLPixel((xTMS9118<<3)+5, (yTMS9118<<3)+i,fgCol);
      c=c-4;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+5, (yTMS9118<<3)+i,bgCol);

    if(c >= 2) {
      plotSDLPixel((xTMS9118<<3)+6, (yTMS9118<<3)+i,fgCol);
      c=c-2;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+6, (yTMS9118<<3)+i,bgCol);

    if(c >= 1) {
      plotSDLPixel((xTMS9118<<3)+7, (yTMS9118<<3)+i,fgCol);
      c=c-1;
    }
   else 
      plotSDLPixel((xTMS9118<<3)+7, (yTMS9118<<3)+i,bgCol);
  }
}

/* ******************************************************************************************************************************
   | draw entire screen worth of characters from RAM to SDL window                                                              |
   ******************************************************************************************************************************
*/
void syncNameTableToSDLWindow(void) 
{
  static int x;
  static int y;

  for(x=0;x<32;x++) {
    for(y=0;y<24;y++) {
      plotCharOnSDLPixels(x,y,getCharFromNameTable(x,y));
    }
  }
}

/* ******************************************************************************************************************************
   | draw sprites onto SDL window                                                                                               |
   ******************************************************************************************************************************
*/
void drawSpritesOnSDLWindow(void) 
{
  static int x;
  static int y;
  static Uint32 addr;

  for(int i=32;i>=0;--i) {
      if(spriteAttrArray[i].patt != -1 && spriteAttrArray[i].color != TRANSPARENT) {
        addr = g.SpritePatternTableAddr;

        if(g.sprites16x16Enabled == TRUE) 
            addr = addr + (spriteAttrArray[i].patt << 5);
        else
            addr = addr + (spriteAttrArray[i].patt << 3);

          Uint32 clr = getARGBFromTMS9118Color(spriteAttrArray[i].color);
          x = spriteAttrArray[i].x;
          y = spriteAttrArray[i].y;
          for(int j=0;j<16;++j) {
            byte c = screen_buffer[addr+j];

            if(c >= 128) {
                plotSDLPixel(x,y+j,clr);
                c=c-128;
            }
            if(c >= 64) {
                plotSDLPixel(x+1, y+j,clr);
                c=c-64;
            }
            if(c >= 32) {
                plotSDLPixel(x+2, y+j,clr);
                c=c-32;
            }
            if(c >= 16) {
                plotSDLPixel(x+3, y+j,clr);
                c=c-16;
            }
            if(c >= 8) {
                plotSDLPixel(x+4, y+j,clr);
                c=c-8;
            }
            if(c >= 4) {
                plotSDLPixel(x+5, y+j,clr);
                c=c-4;
            }
            if(c >= 2) {
                plotSDLPixel(x+6, y+j,clr);
                c=c-2;
            }
            if(c >= 1) {
                plotSDLPixel(x+7, y+j,clr);
                c=c-1;
            }
          }
          for(int j=16;j<32;++j) {
            byte c = screen_buffer[addr+j];

            if(c >= 128) {
                plotSDLPixel(x+8, y+j-16,clr);
                c=c-128;
            }
            if(c >= 64) {
                plotSDLPixel(x+9, y+j-16,clr);
                c=c-64;
            }
            if(c >= 32) {
                plotSDLPixel(x+10, y+j-16,clr);
                c=c-32;
            }
            if(c >= 16) {
                plotSDLPixel(x+11, y+j-16,clr);
                c=c-16;
            }
            if(c >= 8) {
                plotSDLPixel(x+12, y+j-16,clr);
                c=c-8;
            }
            if(c >= 4) {
                plotSDLPixel(x+13, y+j-16,clr);
                c=c-4;
            }
            if(c >= 2) {
                plotSDLPixel(x+14, y+j-16,clr);
                c=c-2;
            }
            if(c >= 1) {
                plotSDLPixel(x+15, y+j-16,clr);
                c=c-1;
            }
          }
      }
  }
}

/* ******************************************************************************************************************************
   | paint entire screen on SDL window                                                                                          |
   ******************************************************************************************************************************
*/
void updateEmulatedVDPScreen(void) 
{
  paintBGColorOnSDLWindow();
  syncNameTableToSDLWindow();
  drawSpritesOnSDLWindow();
}


/* ******************************************************************************************************************************
   | Setup SDL and turn on fluidsynth if running on Mac                                                                         |
   ******************************************************************************************************************************
*/
void SDLSetup(void) 
{
   static int sample_nr;
   if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("could not initialize sdl2: %s\n", SDL_GetError());
    exit(-2);
   }
   pixels = malloc(sizeof(Uint32) * (SCREEN_WIDTH*SCREEN_HEIGHT));

   window = SDL_CreateWindow(
      "PACMAN",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      SCREEN_WIDTH, SCREEN_HEIGHT,
      SDL_WINDOW_SHOWN
   );
   if (window == NULL) {
     printf("could not create window: %s\n", SDL_GetError());
     exit(-3);
   }

   renderer = SDL_CreateRenderer(window, -1, 0);
   texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
   memset(pixels, 255, SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(Uint32));

   settings = new_fluid_settings();
   if(settings == NULL) {
       printf("Unable to open fluidsynth settings object.\n");
       exit(-1);
   }

   synth = new_fluid_synth(settings);
   if(synth == NULL)
   {
       printf("Unable to open fluidsynth synth object.\n");
       exit(-2);
   }

   if(fluid_synth_sfload(synth, "gm.sf2", 1) == -1)
   {
       printf("Unable to load 'gm.sf2' sound font into fluidsynth.\n");
       exit(-3);
   }

   adriver = new_fluid_audio_driver(settings, synth);
   if(adriver == NULL)
   {
       printf("Unable to create audio_driver.\n");
       exit(-4);
   }

    // set the program change to 81 (synthesizer) for all channels
   for(int i=0;i<16;++i)
      fluid_synth_program_change(synth, i, 81);
}


/* ******************************************************************************************************************************
   | Shutdown SDL                                                                                                               |
   ******************************************************************************************************************************
*/
void SDLShutdown(void) 
{
   SDL_DestroyTexture(texture);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
   delete_fluid_audio_driver(adriver);
   delete_fluid_synth(synth);
   delete_fluid_settings(settings);
}

/* ******************************************************************************************************************************
   | Emulate TMS9118 hardware on SDL                                                                                            |
   ******************************************************************************************************************************
*/
void emulateTMS9118HardwareUpdate(void) 
{
    updateEmulatedVDPScreen();
    SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    hold(300);
}

#endif


/* ******************************************************************************************************************************
   | VDP register set (low level register change in TMS9918)                                                                    |
   ******************************************************************************************************************************
*/
void setVDPRegister(byte reg,byte dat) {
#ifdef GCC_COMPILED
    VDPRegisters[reg] = dat;
#else
    PORTX81 = dat;
    reg = reg+128;
    PORTX81 = reg;
#endif
}


/* ******************************************************************************************************************************
   | Set byte in VDP RAM                                                                                                        |
   ******************************************************************************************************************************
*/
void setVDPRAM(unsigned int addr, byte dat) {
#ifdef GCC_COMPILED
    screen_buffer[addr] = dat;
#else
    static unsigned int addr_2;
    static unsigned int addr_1;
    
    addr_1 = (addr >> 8);
    addr_2 = addr_1;
    addr_2 = addr_2 + 64;

    addr_1 = (addr_1 << 8);
    addr_1 = addr - addr_1;

    PORTX81 = (byte)addr_1;
    PORTX81 = (byte)addr_2;
    PORTX80 = dat;
#endif
}

/* ******************************************************************************************************************************
   | get left or right joystick position                                                                                        |
   ******************************************************************************************************************************
*/
#define getFastLeftJoystick() PORTX00
#define getFastRightJoystick() PORTX01

byte getJoystick(byte LeftOrRight) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
#ifdef GCC_COMPILED 
    char ret=0;
    SDL_PumpEvents();
    const Uint8 *keystates  = SDL_GetKeyboardState( NULL );
    if(LeftOrRight == LEFT_POS) {
      if(keystates[SDL_SCANCODE_UP])
        return J_N;
      if(keystates[SDL_SCANCODE_DOWN])
        return J_S;
      if(keystates[SDL_SCANCODE_LEFT])
        return J_W;
      if(keystates[SDL_SCANCODE_RIGHT])
        return J_E;
      if(keystates[SDL_SCANCODE_RETURN])
        return J_BUTTON;
    }

    if(LeftOrRight == RIGHT_POS) {
      if(keystates[SDL_SCANCODE_I])
        return J_N;
      if(keystates[SDL_SCANCODE_M])
        return J_S;
      if(keystates[SDL_SCANCODE_J])
        return J_W;
      if(keystates[SDL_SCANCODE_K])
        return J_E;
      if(keystates[SDL_SCANCODE_SPACE])
        return J_BUTTON;
    }

    if(keystates[SDL_SCANCODE_ESCAPE])
      gameLoopRunning = FALSE;

    return 0;
#else
    if(LeftOrRight == RIGHT_POS)
        return PORTX01;
    else
        return PORTX00;
#endif
}

/* ******************************************************************************************************************************
   | Put character on screen at position                                                                                        |
   ******************************************************************************************************************************
*/
void setCharInNameTable(byte x, byte y, byte c) 
{
    static unsigned int addr;
    addr = g.NameTableAddr;
    addr = addr + (y<<5) + x;

    setVDPRAM(addr,c);
}


/* ******************************************************************************************************************************
   | Write a string at screen position                                                                                          |
   ******************************************************************************************************************************
*/
void setCharsInNameTable(byte x, byte y, char* s) 
{
    static unsigned int addr;
    static byte c;

    addr = g.NameTableAddr;
    addr = addr + (y<<5) + x;

    for(int i=0;i<strlen(s);++i) {
        c = *(s+i);
        setVDPRAM(addr+i, c);
    }
}

/* ******************************************************************************************************************************
   | load pattern table with binary data                                                                                        |
   ******************************************************************************************************************************
*/
void setPatternTableEntry(byte pos, char* p, int l) 
{
  static unsigned int i;
  static unsigned int addr;

  addr = (pos<<3) + g.PatternTableAddr;

  for(i=0;i<l;++i)
    setVDPRAM(addr+i, *(p+i));
}

/* ******************************************************************************************************************************
   | set foreground and background of a group of characters                                                                     |
   ******************************************************************************************************************************
*/
byte setColorTableEntry(byte colorGroup, byte foreground, byte background) 
{
    static byte c;

    if(colorGroup > 31 || foreground > WHITE || background > WHITE)
        return 0; 

    c = ((foreground << 4) & 0xf0) + (background & 0x0f);
    setVDPRAM(g.ColorTableAddr + colorGroup, c);
}

/* ******************************************************************************************************************************
   | load character patterns                                                                                                    |
   ******************************************************************************************************************************
*/
void setPatterns(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
   static unsigned int i;
   
   static char p0[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char a[] =  {0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81};
   static char b[] =  {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char c[] =  {0x3c,0x42,0x81,0x81,0x81,0x81,0x81,0x81};
   static char d[] =  {0x3F,0x40,0x80,0x80,0x80,0x80,0x40,0x3F};
   static char e[] =  {0x81,0x81,0x81,0x81,0x81,0x81,0x42,0x3C};
   static char f[] =  {0xFC,0x02,0x01,0x01,0x01,0x01,0x02,0xFC};
   static char g[] =  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char h[] =  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80};
   static char ii[] = {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char j[] =  {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
   static char k[] =  {0x3F,0x40,0x80,0x80,0x80,0x80,0x80,0x80};
   static char l[] =  {0xFC,0x02,0x01,0x01,0x01,0x01,0x01,0x01};
   static char m[] =  {0x01,0x01,0x01,0x01,0x01,0x01,0x02,0xFC};
   static char n[] =  {0x80,0x80,0x80,0x80,0x80,0x80,0x40,0x3F};
   
   static char r[] =  {0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00};
   static char q[] =  {0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char s[] =  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char t[] =  {0xc0,0xe0,0xf0,0xf0,0xe0,0xc0,0x00,0x00};
   static char u[] =  {0x00,0x01,0x03,0x03,0x01,0x00,0x00,0x00};
   static char x[] =  {0x38,0x7C,0x3E,0x0E,0x3E,0x7C,0x38,0x00};

   static char AT[] = {0x3C,0x42,0x99,0xA1,0xA1,0x99,0x42,0x3C};
   static char AU[] = {0x78,0x84,0x84,0xFC,0x84,0x84,0x84,0x00};
   static char BU[] = {0xF8,0x84,0x84,0xF8,0x84,0x84,0xF8,0x00};
   static char CU[] = {0x78,0x84,0x80,0x80,0x80,0x84,0x78,0x00};
   static char DU[] = {0xF8,0x84,0x84,0x84,0x84,0x84,0xF8,0x00};
   static char EU[] = {0xFC,0x80,0x80,0xF8,0x80,0x80,0xFC,0x00};
   static char FU[] = {0xFC,0x80,0x80,0xF8,0x80,0x80,0x80,0x00};
   static char GU[] = {0x78,0x80,0x80,0xBC,0x84,0x84,0x78,0x00};
   static char HU[] = {0x84,0x84,0x84,0xFC,0x84,0x84,0x84,0x00};
   static char IU[] = {0xFC,0x30,0x30,0x30,0x30,0x30,0xFC,0x00};
   static char JU[] = {0x7C,0x10,0x10,0x10,0x90,0x90,0x70,0x00};
   static char KU[] = {0x84,0x98,0xA0,0xC0,0xA0,0x98,0x84,0x00};
   static char LU[] = {0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x00};
   static char MU[] = {0x84,0xCC,0xB4,0xB4,0x84,0x84,0x84,0x00};
   static char NU[] = {0x84,0xC4,0xA4,0xA4,0x94,0x8C,0x84,0x00};
   static char OU[] = {0x78,0x84,0x84,0x84,0x84,0x84,0x78,0x00};
   static char PU[] = {0xF8,0x84,0x84,0xF8,0x80,0x80,0x80,0x00};
   static char QU[] = {0x78,0x84,0x84,0x84,0x94,0x8C,0x7C,0x00};
   static char RU[] = {0xF8,0x84,0x88,0xF0,0x88,0x88,0x84,0x00};
   static char SU[] = {0x7C,0x80,0x80,0x78,0x04,0x04,0xF8,0x00};
   static char TU[] = {0xFC,0x30,0x30,0x30,0x30,0x30,0x30,0x00};
   static char UU[] = {0x84,0x84,0x84,0x84,0x84,0x84,0x78,0x00};
   static char VU[] = {0x84,0x84,0x84,0x48,0x48,0x48,0x30,0x00};
   static char WU[] = {0x84,0x84,0x84,0xB4,0xB4,0xCC,0x84,0x00};
   static char XU[] = {0x84,0x84,0x48,0x30,0x48,0x84,0x84,0x00};
   static char YU[] = {0x84,0x48,0x48,0x30,0x30,0x30,0x30,0x00};
   static char ZU[] = {0xFC,0x08,0x10,0x10,0x20,0x40,0xFC,0x00};
   static char N0[] = {0x78,0x8C,0x94,0xB4,0xA4,0xC4,0x78,0x00};
   static char N1[] = {0x30,0x70,0x30,0x30,0x30,0x30,0xFC,0x00};
   static char N2[] = {0x78,0x84,0x84,0x18,0x60,0x80,0xFC,0x00};
   static char N3[] = {0x78,0x84,0x04,0x38,0x04,0x84,0x78,0x00};
   static char N4[] = {0xCC,0xCC,0xCC,0xFC,0x0C,0x0C,0x0C,0x00};
   static char N5[] = {0xFC,0x80,0x80,0x78,0x04,0x84,0x78,0x00};
   static char N6[] = {0x38,0x40,0x80,0xF8,0x84,0x84,0x78,0x00};
   static char N7[] = {0xFC,0x0C,0x18,0x18,0x30,0x30,0x60,0x00};
   static char N8[] = {0x78,0x84,0x84,0x78,0x84,0x84,0x78,0x00};
   static char N9[] = {0x78,0x84,0x84,0x7C,0x04,0x08,0x70,0x00};
   static char NC[] = {0x00,0x30,0x30,0x00,0x00,0x30,0x30,0x00};
   static char ND[] = {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00};
   static char flag[] ={0x8E,0xFE,0xFE,0xFE,0x72,0x02,0x02,0x02};
   static char eq[] =  {0x00,0x00,0x00,0xFE,0x00,0xFE,0x00,0x00};
   
   for(i=0;i<256;++i)                   // set all characters to blank
          setPatternTableEntry(i,p0,8);

   setPatternTableEntry('@',AT,8);
   setPatternTableEntry('A',AU,8);      
   setPatternTableEntry('B',BU,8);      
   setPatternTableEntry('C',CU,8);      
   setPatternTableEntry('D',DU,8);      
   setPatternTableEntry('E',EU,8);      
   setPatternTableEntry('F',FU,8);      
   setPatternTableEntry('G',GU,8);      
   setPatternTableEntry('H',HU,8);      
   setPatternTableEntry('I',IU,8);      
   setPatternTableEntry('J',JU,8);      
   setPatternTableEntry('K',KU,8);      
   setPatternTableEntry('L',LU,8);      
   setPatternTableEntry('M',MU,8);      
   setPatternTableEntry('N',NU,8);      
   setPatternTableEntry('O',OU,8);      
   setPatternTableEntry('P',PU,8);      
   setPatternTableEntry('Q',QU,8);      
   setPatternTableEntry('R',RU,8);      
   setPatternTableEntry('S',SU,8);      
   setPatternTableEntry('T',TU,8);      
   setPatternTableEntry('U',UU,8);      
   setPatternTableEntry('V',VU,8);      
   setPatternTableEntry('W',WU,8);      
   setPatternTableEntry('X',XU,8);      
   setPatternTableEntry('Y',YU,8);      
   setPatternTableEntry('Z',ZU,8);      
                                       //     SHAPE  
   setPatternTableEntry('a',a,8);      //      | |   
                                       //      | |   
   setPatternTableEntry('b',b,8);      //      ___  
                                       //      ---   
   setPatternTableEntry('c',c,8);      //      .--.
                                       //      |  |  
   setPatternTableEntry('d',d,8);      //       ___
                                       //      {___  
   setPatternTableEntry('e',e,8);      //      |  |
                                       //      '--'  
   setPatternTableEntry('f',f,8);      //       ___
                                       //       ___}                 
   setPatternTableEntry('g',g,8);      //       ___

   setPatternTableEntry('h',h,8);      //       |
                                       //       |
   setPatternTableEntry('i',ii,8);     //       --
                                       //
   setPatternTableEntry('j',j,8);      //         |
                                       //         |
   setPatternTableEntry('k',k,8);      //       ___
                                       //      |
   setPatternTableEntry('l',l,8);      //       ___
                                       //          |
   setPatternTableEntry('m',m,8);      //          |
                                       //       ___|
   setPatternTableEntry('n',n,8);      //       |
                                       //       |___
   setPatternTableEntry('r',r,8);      //          .
   setPatternTableEntry('q',q,8);      //          _
   setPatternTableEntry('s',s,8);      //          _

   setPatternTableEntry(ENERGIZER_RIGHT_CHAR,t,8);      //       energizer (right)
   setPatternTableEntry(ENERGIZER_LEFT_CHAR,u,8);      //       energizer (left)

   setPatternTableEntry('x',x,8);      //       small pac-man

   setPatternTableEntry('0',N0,8); 
   setPatternTableEntry('1',N1,8); 
   setPatternTableEntry('2',N2,8); 
   setPatternTableEntry('3',N3,8); 
   setPatternTableEntry('4',N4,8); 
   setPatternTableEntry('5',N5,8); 
   setPatternTableEntry('6',N6,8); 
   setPatternTableEntry('7',N7,8); 
   setPatternTableEntry('8',N8,8); 
   setPatternTableEntry('9',N9,8); 
   setPatternTableEntry(':',NC,8); 
   setPatternTableEntry('-',ND,8);
   setPatternTableEntry('=',eq,8);

   static char BLK1[] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF}; // characters for the attract screen
   static char BLK2[] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
   static char BLK3[] = {0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80};
   static char BLK4[] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
   static char BLK5[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

   setPatternTableEntry('(', BLK1,8);
   setPatternTableEntry(')', BLK2,8);
   setPatternTableEntry('*', BLK3,8);
   setPatternTableEntry('+', BLK4,8);
   setPatternTableEntry(',', BLK5,8);

   setPatternTableEntry(FLAG_CHAR,flag,8);
}

/* ******************************************************************************************************************************
   | set TMS9118 hardware registers and graphic structure                                                                       |
   ******************************************************************************************************************************
*/
void setGraphicsMode(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static byte b;

  b = 0;
  if(g.graphicsMode == GRAPHICSMODE2) 
      b = b | 1;
 
  if(g.externalVideoEnabled == TRUE)
      b = b | 2;

  setVDPRegister(0,b);

  b = 192;
  if(g.interruptsEnabled == TRUE)
      b = b | 32;

  if(g.graphicsMode == TEXTMODE)
      b = b | 16;
  
  if(g.graphicsMode == MULTICOLORMODE) 
      b = b | 8;

  if(g.sprites16x16Enabled == TRUE)
      b = b | 2;

  if(g.spritesMagnified == TRUE) 
      b = b | 1;
  
  setVDPRegister(1, b);
  b = 0;

  setVDPRegister(2, 0xf);
  g.NameTableAddr = 0x3c00;

  setVDPRegister(3, 0x80);
  g.ColorTableAddr = 0x2000;

  setVDPRegister(4, 0x0);
  g.PatternTableAddr = 0x0000;

  setVDPRegister(5, 0x70);
  g.spriteAttrStructTableAddr = 0x3800;

  setVDPRegister(6, 0x03);
  g.SpritePatternTableAddr = 0x1800;

  b = g.textForegroundColor << 4 | g.backgroundColor;
  setVDPRegister(7, b);
}

/* ******************************************************************************************************************************
   | Load sprite pattern into pattern table                                                                                     |
   ******************************************************************************************************************************
*/
void setSpritePatternByArray(byte patternNumber, char* p, char l) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static unsigned int addr;
  static unsigned int i;

  addr = g.SpritePatternTableAddr;
  if(g.sprites16x16Enabled == TRUE) 
      addr = addr + (patternNumber << 5);
  else
      addr = addr + (patternNumber << 3);

  for(i=0;i<l;++i) {
    setVDPRAM(addr+i,*(p+i));
  }
}




/* ******************************************************************************************************************************
   | Force all 32 sprites to be transparent                                                                                     |
   ******************************************************************************************************************************
*/
void forceAllSpritesTransparent(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static unsigned int addr;

    for(int spriteNum = 0;spriteNum <= 32; spriteNum++) {
      addr = g.spriteAttrStructTableAddr + (spriteNum << 2);
      setVDPRAM(addr, (byte)   0);
      setVDPRAM(addr+1, (byte) 0);
      setVDPRAM(addr+2, (byte) 0);
      setVDPRAM(addr+3, (byte) TRANSPARENT);
    }
}


/* ******************************************************************************************************************************
   | move sprite data out of sprite attribute struct and move it into registers                                                 |
   ******************************************************************************************************************************
*/
void syncSpriteAttrStructsToHardware(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static unsigned int addr;
  static byte vert;
  static int horiz;
  static byte b5;
  static int x;
  static unsigned int y;
  static byte patt;

  for(int spriteNum = 0;spriteNum <= MAX_SPRITENUM; spriteNum++) {
      patt = spriteAttrArray[spriteNum].patt;
      x = spriteAttrArray[spriteNum].x;
      y = spriteAttrArray[spriteNum].y;
      addr = g.spriteAttrStructTableAddr + (spriteNum << 2);

      b5 = spriteAttrArray[spriteNum].color;
      
      vert = y ;

      if(x<0) {
        b5 = b5 | 0x80;
        x = x + 32;
      }
      horiz = x ;

      setVDPRAM(addr, vert);
      setVDPRAM(addr+1, (byte)horiz);
      setVDPRAM(addr+2, patt<<2);
      setVDPRAM(addr+3, b5);
  }
}


/* ******************************************************************************************************************************
   | clear TRS-80 screen                                                                                                        |
   ******************************************************************************************************************************
*/
void clearTRSScreen(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  #ifdef GCC_COMPILED
      for(int i=0;i<40;++i) printf("\n");
  #else
      void* p = 0x3c00;
      memset(p, ' ', 0x3ff);
  #endif
}


/* ******************************************************************************************************************************
   | draw dots into maze                                                                                                        |
   ******************************************************************************************************************************
*/
void drawDots(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static byte k;

  for(k=2;k<31;++k) {
    setCharInNameTable(k,3,'r');
    setCharInNameTable(k,23,'q');
  }
  setCharInNameTable(1,23,'s');

  for(k=3;k<23;++k) {
    setCharInNameTable(2,k,'r');
    setCharInNameTable(30,k,'r');
    setCharInNameTable(5,k,'r');
    setCharInNameTable(27,k,'r');
    if(k < 21) {
      setCharInNameTable(8,k,'r');
      setCharInNameTable(24,k,'r');
    }
  } 

  for(k=5;k<27;++k) 
    setCharInNameTable(k,20,'r');

  for(k=9;k<24;++k) {
    setCharInNameTable(k,17,'r');
    setCharInNameTable(k,6,'r');
  }
  for(k=2;k<5;++k) {
    setCharInNameTable(k,12,'r');
    setCharInNameTable(k+25,12,'r');
  }
  for(k=17;k<21;++k) {
    setCharInNameTable(16,k,'r');
    setCharInNameTable(16,k-11,'r');
  }
  for(k=11;k<22;++k) {
    setCharInNameTable(k,9,'r');
    setCharInNameTable(k,14,'r');
  }
  for(k=9;k<14;++k) {
    setCharInNameTable(11,k,'r');
    setCharInNameTable(21,k,'r');
  }
  for(k=8;k<12;++k) {
    setCharInNameTable(k,12,'r');
    setCharInNameTable(k+13,12,'r');
  }
  for(k=15;k<=17;++k) 
    setCharInNameTable(k,17,' ');

  for(k=15;k<17;++k) {
    setCharInNameTable(13,k,'r');
    setCharInNameTable(19,k,'r');
  }

  setCharInNameTable(2,5,ENERGIZER_RIGHT_CHAR);   // energizer (right side)
  setCharInNameTable(2,19,ENERGIZER_RIGHT_CHAR);  // energizer (right side)
  setCharInNameTable(30,5,ENERGIZER_RIGHT_CHAR);  // energizer (right side)
  setCharInNameTable(30,19,ENERGIZER_RIGHT_CHAR); // energizer (right side)
  setCharInNameTable(1,5,ENERGIZER_LEFT_CHAR);  // energizer (left side)
  setCharInNameTable(1,19,ENERGIZER_LEFT_CHAR); // energizer (left side)
  setCharInNameTable(29,5,ENERGIZER_LEFT_CHAR); // energizer (left side)
  setCharInNameTable(29,19,ENERGIZER_LEFT_CHAR); // energizer (left side)

  setColorTableEntry(17,DARKGREEN,BLACK);
  if(levelCtr < 6) {
      for(int i=0;i<levelCtr;++i) {
               setCharInNameTable(16 - i,0,FLAG_CHAR);
      }
  }
  else {
    sprintf(buf,"%c=%d   ", FLAG_CHAR,levelCtr);
    setCharsInNameTable(12,0,buf);
  }

}

/* ******************************************************************************************************************************
   | make all the characters white (except the ones in the alphabetic text)                                                     |
   ******************************************************************************************************************************
*/
void setEverythingWhite(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static byte j;

  for(j=0;j<32;++j)
     setColorTableEntry(j, WHITE, BLACK);  // set all characters to blue on black
  for(j=8;j<=11;++j)
     setColorTableEntry(j, DARKRED, BLACK);   // set chars 64 - 95 to red on black 
}

/* ******************************************************************************************************************************
   | set the color groups at the right colors for the maze                                                                      |
   ******************************************************************************************************************************
*/
void setMazeColors(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static byte j;

  for(j=0;j<32;++j)
     setColorTableEntry(j, DARKBLUE, BLACK);  // set all characters to blue on black
  for(j=8;j<=11;++j)
     setColorTableEntry(j, DARKRED, BLACK);   // set chars 64 - 95 to red on black 
  for(j=6;j<=7;++j)
     setColorTableEntry(j, WHITE, BLACK);     // set chars 48 - 63 to white on black

   setColorTableEntry(14,WHITE,BLACK); // dots
   setColorTableEntry(15,DARKYELLOW,BLACK); // little pacmen
   setColorTableEntry(16,WHITE,BLACK); // energizers
}

/* ******************************************************************************************************************************
   | Draw pacman lives icons at top                                                                                             |
   ******************************************************************************************************************************
*/
void drawPacmanLives(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  setColorTableEntry(15,DARKYELLOW,BLACK);
  setCharsInNameTable(2,0,"          ");
  for(int i=0;i<lives;++i) {
    setCharInNameTable(i+2,0,'x');
  }
}


/* ********************************************************************************************************************************
   | update the score and show it on the screen (sprintf is expensive, so we don't do it all the time -- just when score updates) |
   ********************************************************************************************************************************
*/
void updateScore(int incr) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static unsigned long oldscore = 0;

  score = score + incr;
  sprintf(buf, "%08ld", score);
  setCharsInNameTable(24,0,buf);

  // free guy routine
  if(lives < 10) {
      if((oldscore < 10000 && score >= 10000) || 
         (oldscore < 50000 && score >= 50000) ||
         (oldscore < 100000 && score >= 100000) || 
         (oldscore < 500000 && score >= 500000) ||
         (oldscore < 1000000 && score >= 1000000) )
      {
        ++lives;
        drawPacmanLives();

        // play some beeps when you get a free guy by putting the beeps onto the circular buffer and letting the main
        // game loop pick them up
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, -1);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, -1);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, -1);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, -1);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, -1);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
      }
  }
  oldscore = score;
}


/* ********************************************************************************************************************************
   | draw characters of maze                                                                                                      |
   ********************************************************************************************************************************
*/
void drawMaze(void) {
  static byte j;
  static byte k;

  setMazeColors();

  // clear screen
  for(k=0;k<24;++k)
      for(j=0;j<32;++j)
          setCharInNameTable(j,k,' ');

   for(j=3;j<=9;++j) {
       setCharInNameTable(0,j,'a');
       setCharInNameTable(31,j,'a');
   }

   for(j=14;j<=22;++j) {
       setCharInNameTable(0,j,'a');
       setCharInNameTable(31,j,'a');    
   }

   for(j=0;j<32;++j) {
       setCharInNameTable(j,1,'g');
   }

   setCharsInNameTable(18,0,"SCORE:");
   updateScore(0);

   for(j=5;j<=9;++j) {
       setCharInNameTable(3, j, 'a');
       setCharInNameTable(28, j, 'a');
   }

   for(j=14;j<=20;++j) {
       setCharInNameTable(3, j, 'a');
       setCharInNameTable(28, j, 'a');
   }

   for(j=5;j<=17;++j) {
       setCharInNameTable(6, j, 'a');
       setCharInNameTable(25, j, 'a');
   }

   for(j=8;j<=9;++j) {
       setCharInNameTable(9, j, 'a');
       setCharInNameTable(22,j, 'a');
   }

   setCharInNameTable(9,14,'a');
   setCharInNameTable(22,14,'a');

   for(j=10;j<22;++j) 
       setCharInNameTable(j,4,'b');
   
   for(j=10;j<=13;++j) {
       setCharInNameTable(j,7,'b');
       setCharInNameTable(j+8,7,'b');
   }

   setCharInNameTable(13, 10, 'i');
   setCharInNameTable(14, 10, 'i');
   setCharInNameTable(17, 10, 'i');
   setCharInNameTable(18, 10, 'i');
   setCharInNameTable(12, 11, 'h');
   setCharInNameTable(19, 11, 'j');

   for(j=13;j<=18;++j) 
      setCharInNameTable(j,12, 'g');

   setCharInNameTable(10,15, 'b');
   setCharInNameTable(21,15, 'b');
   setCharInNameTable(15,15, 'b');
   setCharInNameTable(16,15, 'b');

   for(j=10;j<=13;++j) {
    setCharInNameTable(j,18, 'b');
    setCharInNameTable(j+8,18, 'b');
   }

   for(j=7;j<=24;++j) 
    setCharInNameTable(j,21,'b');

  setCharInNameTable(0,2,'c');
  setCharInNameTable(3,4,'c');
  setCharInNameTable(6,4,'c');
  setCharInNameTable(25,4,'c');
  setCharInNameTable(28,4,'c');
  setCharInNameTable(31,2,'c');
  setCharInNameTable(0,13,'c');
  setCharInNameTable(3,13,'c');
  setCharInNameTable(9,13,'c');
  setCharInNameTable(22,13,'c');
  setCharInNameTable(28,13,'c');
  setCharInNameTable(31,13,'c');

  setCharInNameTable(0,10,'e');
  setCharInNameTable(3,10,'e');
  setCharInNameTable(9,10,'e');
  setCharInNameTable(22,10,'e');
  setCharInNameTable(28,10,'e');
  setCharInNameTable(31,10,'e');
  setCharInNameTable(0,23,'e');
  setCharInNameTable(3,21,'e');
  setCharInNameTable(6,18,'e');
  setCharInNameTable(25,18,'e');
  setCharInNameTable(31,23,'e');
  setCharInNameTable(28,21,'e');

  setCharInNameTable(9, 4, 'd');
  setCharInNameTable(9, 18, 'd');
  setCharInNameTable(6, 21, 'd');
  setCharInNameTable(17,18, 'd');
  setCharInNameTable(20,15, 'd');
  setCharInNameTable(14,15, 'd');
  setCharInNameTable(17,7,'d');

  setCharInNameTable(22,4,'f');
  setCharInNameTable(17,15,'f');
  setCharInNameTable(14,18,'f');
  setCharInNameTable(22,18,'f');
  setCharInNameTable(25,21,'f');
  setCharInNameTable(11,15,'f');
  setCharInNameTable(14,7,'f');

  setCharInNameTable(12,10,'k');
  setCharInNameTable(9,7,'k');
  setCharInNameTable(19,10,'l');
  setCharInNameTable(22,7,'l');
  setCharInNameTable(12,12,'n');
  setCharInNameTable(9,15,'n');
  setCharInNameTable(19,12,'m');
  setCharInNameTable(22,15,'m');

  setCharInNameTable(9,15,'n');
  setCharInNameTable(22,15,'m');
  
  drawDots();  

  drawPacmanLives();
}

/* ******************************************************************************************************************************
   | can this sprite go south?                                                                                                  |
   ******************************************************************************************************************************
*/
byte canGoSouth(byte spriteNum) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static int x;
    static int y;

    x = spriteAttrArray[spriteNum].x;
    y = spriteAttrArray[spriteNum].y;

    if(y < 174) 
      if(x == 8 || x == 32 || x == 208 || x == 232)
        return TRUE;

    if(y <= 150) 
      if(x == 56 || x == 184)
        return TRUE;

    if(x == 120) {
      if(y >= 40 && y < 64)
        return TRUE;
      if(y >= 128 && y < 152)
        return TRUE;
    }

    if(y >= 64 && y < 104)
      if(x == 80 || x == 160)
        return TRUE;

    if(y >= 104 && y < 128) 
      if(x == 96 || x == 144)
        return TRUE;

    if(x == 120) {
      if(y >= 64 && y < 84)
        return TRUE;
    }

    return FALSE;
}


/* ******************************************************************************************************************************
   | can this sprite go north?                                                                                                  |
   ******************************************************************************************************************************
*/
byte canGoNorth(byte spriteNum) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static int x;
    static int y;

    x = spriteAttrArray[spriteNum].x;
    y = spriteAttrArray[spriteNum].y;

    if(y > 16) 
      if(x == 8 || x == 32 || x == 208 || x == 232)
        return TRUE;

    if(y <= 150 && y > 16) 
      if(x == 56 || x == 184)
        return TRUE;

    if(x == 120) {
      if(y > 40 && y <= 84)
        return TRUE;
      if(y > 128 && y <= 152)
        return TRUE;
    }

    if(y > 64 && y <= 104) 
      if(x == 80 || x == 160)
        return TRUE;

    if(y > 104 && y <= 128) 
      if(x == 96 || x == 144)
        return TRUE;

    if(y == 152) 
      if(x == 184 || x == 56)
        return TRUE;

    return FALSE;
}

/* ******************************************************************************************************************************
   | can this sprite go east?                                                                                                   |
   ******************************************************************************************************************************
*/
byte canGoEast(byte spriteNum) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static int x;
    static int y;

    x = spriteAttrArray[spriteNum].x;
    y = spriteAttrArray[spriteNum].y;

    if(x >= 248)
      return FALSE;

    if(y == 16 || y == 174) 
      if(x >= 8 && x <= 230)
        return TRUE;
    
    if(y == 40 || y == 128) 
      if(x >= 56 && x <= 182)
        return TRUE;    

    if(y == 64 || y == 104) 
      if(x >= 80 && x < 160)
        return TRUE;

    if(y == 152) 
      if(x >= 32 && x < 208)
        return TRUE;

    if(y == 88) {
      if(x < 32 && x >= 0)
        return TRUE;
      if(x < 80 && x>= 56)
        return TRUE;
      if(x >= 160 && x < 184)
        return TRUE;
      if(x >= 208 && x < 240)
        return TRUE;
    }

    if(y == 84) {
      if(x >= 98 && x < 142)
        return TRUE;
    }

    return FALSE;
}

/* ******************************************************************************************************************************
   | can this sprite go west?                                                                                                   |
   ******************************************************************************************************************************
*/
byte canGoWest(byte spriteNum) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static int x;
    static int y;

    x = spriteAttrArray[spriteNum].x;
    y = spriteAttrArray[spriteNum].y;

    if(x <= -8)
      return FALSE;

    if(y == 16 || y == 174) 
      if(x >= 10 && x <= 232)
        return TRUE;
    
    if(y == 40 || y == 128) 
      if(x >= 58 && x <= 184)
        return TRUE;

    if(y == 64 || y == 104) 
      if(x > 80 && x <= 160)
        return TRUE;    

    if(y == 152) 
      if(x > 32 && x <= 208)
        return TRUE;

    if(y == 88) {
      if(x <= 32 && x > 0)
        return TRUE;
      if(x <= 80 && x > 56)
        return TRUE;
      if(x > 160 && x <= 184)
        return TRUE;
      if(x > 208 && x <= 240)
        return TRUE;
    }

    if(y == 84) {
      if(x > 98 && x <= 142)
        return TRUE;
    }

    return FALSE;
}


/* ******************************************************************************************************************************
   | set ghost direction - NORTH                                                                                                |
   ******************************************************************************************************************************
*/
void goNorth(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
        spriteAttrArray[i].ydir = -2;
        spriteAttrArray[i].xdir = 0;
        spriteAttrArray[i].prevdir = NORTH;
        spriteAttrArray[i+4].patt = PATT_NORMAL_GHOST_EYES_N;
}

/* ******************************************************************************************************************************
   | set ghost direction - SOUTH                                                                                                |
   ******************************************************************************************************************************
*/
void goSouth(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
        spriteAttrArray[i].ydir = 2;
        spriteAttrArray[i].xdir = 0;
        spriteAttrArray[i].prevdir = SOUTH;
        spriteAttrArray[i+4].patt = PATT_NORMAL_GHOST_EYES_S;
}

/* ******************************************************************************************************************************
   | set ghost direction - EAST                                                                                                 |
   ******************************************************************************************************************************
*/
void goEast(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
        spriteAttrArray[i].xdir = 2;
        spriteAttrArray[i].ydir = 0;
        spriteAttrArray[i].prevdir = EAST;
        spriteAttrArray[i+4].patt = PATT_NORMAL_GHOST_EYES_E;
}

/* ******************************************************************************************************************************
   | set ghost direction - WEST                                                                                                 |
   ******************************************************************************************************************************
*/
void goWest(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
        spriteAttrArray[i].xdir = -2;
        spriteAttrArray[i].ydir = 0;
        spriteAttrArray[i].prevdir = WEST;
        spriteAttrArray[i+4].patt = PATT_NORMAL_GHOST_EYES_W;
}


/* ******************************************************************************************************************************
   | randomly pick an available ghost direction                                                                                 |
   ******************************************************************************************************************************
*/
void setAvailableGhostDirection(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static int bFoundOne;
  static byte r;
  static int x;
  static int y;
  static int px;
  static int py;

    if(getRand256() > 64) {  // most of the time we'll use this routine that pursues and evades
      x = spriteAttrArray[i].x;
      y = spriteAttrArray[i].y;
      px = spriteAttrArray[PACMAN_SPRITENUM].x;
      py = spriteAttrArray[PACMAN_SPRITENUM].y;

      if(energizerCtr > 0) {
        if(x < px && canGoWest(i)) {
          goWest(i);
          return;
        }
        else
        if(x > px && canGoEast(i)) {
          goEast(i);
          return;
        }
        else
        if(y < py && canGoNorth(i)) {
          goNorth(i);
          return;
        }
        if(y > py && canGoSouth(i)) {
          goSouth(i);
          return;
        }
      }
      else {
        if(x < px && canGoEast(i)) {
          goEast(i);
          return;
        }
        else
        if(x > px && canGoWest(i)) {
          goWest(i);
          return;
        }
        else
        if(y < py && canGoSouth(i)) {
          goSouth(i);
          return;
        }
        if(y > py && canGoNorth(i)) {
          goNorth(i);
          return;
        }
      }
    }

    // however, if we don't use the pursue / evade algorithm we use this random behavior algorithm
    bFoundOne = FALSE;

    while(bFoundOne == FALSE) {
      r = getRand256();
      if(r < 64 && canGoWest(i)) {
        goWest(i);
        bFoundOne = TRUE;  
      }
      else
      if(r < 128 && canGoEast(i)) {
        goEast(i);
        bFoundOne = TRUE;
      }
      else
      if(r < 192 && canGoSouth(i)) {
        goSouth(i);
        bFoundOne = TRUE;
      }
      else
      if(canGoNorth(i)) {
        goNorth(i);
        bFoundOne = TRUE;
      }  
    }


}


/* ******************************************************************************************************************************
   | set ghost colors back to normal                                                                                            |
   ******************************************************************************************************************************
*/
void ghostsNormal(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static int i;
    if(spriteAttrArray[RED_GHOST_SPRITENUM].isEyes == FALSE)
        spriteAttrArray[RED_GHOST_SPRITENUM].color = DARKRED;

    if(spriteAttrArray[BROWN_GHOST_SPRITENUM].isEyes == FALSE)
        spriteAttrArray[BROWN_GHOST_SPRITENUM].color = DARKYELLOW;

    if(spriteAttrArray[CYAN_GHOST_SPRITENUM].isEyes == FALSE)
        spriteAttrArray[CYAN_GHOST_SPRITENUM].color = CYAN;

    if(spriteAttrArray[PINK_GHOST_SPRITENUM].isEyes == FALSE)
        spriteAttrArray[PINK_GHOST_SPRITENUM].color = LIGHTRED;

    for(i=RED_GHOST_EYES_SPRITENUM;i<=BROWN_GHOST_EYES_SPRITENUM;++i) {
      spriteAttrArray[i].color = WHITE;
    }
}

/* ******************************************************************************************************************************
   | manages how ghost eyes get back to the ghost box                                                                           |
   ******************************************************************************************************************************
*/
void eyeBehavior(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static int x;
    static int y;
    static int px;
    static int py;
    static int foundOne;

    x = spriteAttrArray[i].x;
    y = spriteAttrArray[i].y;

    // little alignment cleanups that happen because we made the eyes jump by 4 pixels rather than 2
    if(y == 14) { spriteAttrArray[i].y = 16; y = spriteAttrArray[i].y;}
    if(x == 6)  { spriteAttrArray[i].x = 8;  x = spriteAttrArray[i].x;}
    if(x == 30) { spriteAttrArray[i].x = 32; x = spriteAttrArray[i].x;}

    px = 120;
    py = 84;
    foundOne = FALSE;

    if(x == px && y == py) {
       spriteAttrArray[i].isEyes = FALSE;
       if(i == RED_GHOST_SPRITENUM) spriteAttrArray[i].color = DARKRED;
       if(i == PINK_GHOST_SPRITENUM) spriteAttrArray[i].color = LIGHTRED;
       if(i == BROWN_GHOST_SPRITENUM) spriteAttrArray[i].color = DARKYELLOW;
       if(i == CYAN_GHOST_SPRITENUM) spriteAttrArray[i].color = CYAN;
       return;
    }

    if(x < px && canGoEast(i) && spriteAttrArray[i].prevdir != WEST) {
        goEast(i);
        foundOne = TRUE;
    }
    else
    if(x > px && canGoWest(i) && spriteAttrArray[i].prevdir != EAST) {
        goWest(i);
        foundOne = TRUE;
    }
    else
    if(y < py && canGoSouth(i) && spriteAttrArray[i].prevdir != NORTH) {
        goSouth(i);
        foundOne = TRUE;
    }
    else
    if(y > py && canGoNorth(i) && spriteAttrArray[i].prevdir != SOUTH) {
        goNorth(i);
        foundOne = TRUE;
    }

    if(foundOne == FALSE) {
        if(canGoWest(i)) {
            goWest(i);
            foundOne = TRUE;
        }
        if(canGoNorth(i)) {
            goNorth(i);
            foundOne = TRUE;
        }
    }

    if(foundOne == FALSE) {
        // this shouldn't happen... force eyes into the box
        printf("Ghost %d at (%d,%d), lost, alignment fix probably needed\n",i,spriteAttrArray[i].x,spriteAttrArray[i].y);
        spriteAttrArray[i].isEyes = FALSE;
        spriteAttrArray[i].x = 120;
        spriteAttrArray[i].y = 84;
        return;
    }

    spriteAttrArray[i].x = x + spriteAttrArray[i].xdir*2;
    spriteAttrArray[i].y = y + spriteAttrArray[i].ydir*2;
    spriteAttrArray[i+4].x = spriteAttrArray[i].x; //set eye location
    spriteAttrArray[i+4].y = spriteAttrArray[i].y; //set eye location
}

void normalGhostBehavior(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
      static int ctr = 0;
      static byte r;
      static int x;
      static int y;

      x = spriteAttrArray[i].x;
      y = spriteAttrArray[i].y;

      // this code redirects a ghost if he would get stuck
      if(spriteAttrArray[i].prevdir == NORTH && canGoNorth(i) == FALSE) 
        setAvailableGhostDirection(i);
      else
        if(spriteAttrArray[i].prevdir == SOUTH && canGoSouth(i) == FALSE) 
          setAvailableGhostDirection(i);
        else
          if(spriteAttrArray[i].prevdir == EAST && canGoEast(i) == FALSE) 
            setAvailableGhostDirection(i);
          else 
            if(spriteAttrArray[i].prevdir == WEST && canGoWest(i) == FALSE) 
              setAvailableGhostDirection(i);

      r = getRand256();
      // randomly override current ghost direction every once in a while (if he's not a hat-wearing ghost)
      if(r > 240 && i != GhostWithHat) {
        setAvailableGhostDirection(i);
      }

      // while the ghosts are in the box, make it more likely for them to go north
      if(y == 84 && x >= 114 && x <= 138) {
          if(canGoNorth(i))
            goNorth(i);
      }

      if(r > 72 - (i<<3) )  { // slows down ghosts randomly, with each one having slightly different behavior
          spriteAttrArray[i].x = x + spriteAttrArray[i].xdir;
          spriteAttrArray[i].y = y + spriteAttrArray[i].ydir;
          x = spriteAttrArray[i].x;
          y = spriteAttrArray[i].y;
      }

      if(energizerCtr == 0) {
        // make the ghosts' "legs" swap back and forth if we're in normal mode
        ctr++;
        if( ctr < 24 )  
          spriteAttrArray[i].patt = PATT_NORMAL_GHOST_1;
        else 
          spriteAttrArray[i].patt = PATT_NORMAL_GHOST_2;
        
        if(ctr > 47) ctr = 0;
      }
      else {
        // otherwise make the ghosts appear in their "scared" version
        spriteAttrArray[i].patt = PATT_SCARED_GHOST;
      }

    spriteAttrArray[i+4].x = x; //set eye location
    spriteAttrArray[i+4].y = y; //set eye location

    // make ghosts blue if pac has eaten an energizer
    if(energizerCtr > 0) {
      spriteAttrArray[i+4].color = TRANSPARENT; // turn off eyes

      spriteAttrArray[i].color = DARKBLUE; // make ghost dark blue

      if(energizerCtr > 60 && energizerCtr < 70)
        spriteAttrArray[i].color = WHITE;
      else
      if(energizerCtr > 40 && energizerCtr < 50)
        spriteAttrArray[i].color = WHITE;
      else
      if(energizerCtr > 20 && energizerCtr < 30)
        spriteAttrArray[i].color = WHITE;
      else
      if(energizerCtr > 1 && energizerCtr < 10)
        spriteAttrArray[i].color = WHITE;
    }

    // put hat on someone's head if they have it
    if(i == GhostWithHat) {
      spriteAttrArray[HAT_SPRITENUM].color = DARKGREEN;
      spriteAttrArray[HAT_SPRITENUM].x = x-1;
      spriteAttrArray[HAT_SPRITENUM].y = y-4;
    }

}
/* ******************************************************************************************************************************
   | Move ghosts one step                                                                                                       |
   ******************************************************************************************************************************
*/
void moveGhosts(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static int i;

  for(i=RED_GHOST_SPRITENUM;i<=BROWN_GHOST_SPRITENUM;++i) {
    if(spriteAttrArray[i].isEyes == TRUE)
        eyeBehavior(i);
    else
        normalGhostBehavior(i);
  }

  if(energizerCtr == 1) {
    ghostsNormal();
    ghostCtr = 0;
  }

  if(gameCtr > popScoreGameCtr + 10)
    spriteAttrArray[POP_SCORE_SPRITENUM].color = TRANSPARENT;
}


/* ******************************************************************************************************************************
   | Put ghosts back in their starting spot                                                                                     |
   ******************************************************************************************************************************
*/
void putGhostsInBox(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
   spriteAttrArray[RED_GHOST_SPRITENUM].x = 120;
   spriteAttrArray[RED_GHOST_SPRITENUM].y = 40;
   spriteAttrArray[RED_GHOST_SPRITENUM].color = DARKRED;
   spriteAttrArray[RED_GHOST_SPRITENUM].prevdir = EAST;
   spriteAttrArray[RED_GHOST_SPRITENUM].xdir = 2;
   spriteAttrArray[RED_GHOST_SPRITENUM].ydir = 0;
   spriteAttrArray[RED_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_1;
   spriteAttrArray[RED_GHOST_EYES_SPRITENUM].color = WHITE;
   spriteAttrArray[RED_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;

   spriteAttrArray[CYAN_GHOST_SPRITENUM].x = 130;
   spriteAttrArray[CYAN_GHOST_SPRITENUM].y = 84;
   spriteAttrArray[CYAN_GHOST_SPRITENUM].color = CYAN;
   spriteAttrArray[CYAN_GHOST_SPRITENUM].prevdir = EAST;
   spriteAttrArray[CYAN_GHOST_SPRITENUM].xdir = 2;
   spriteAttrArray[CYAN_GHOST_SPRITENUM].ydir = 0;
   spriteAttrArray[CYAN_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_1;
   spriteAttrArray[CYAN_GHOST_EYES_SPRITENUM].color = WHITE;
   spriteAttrArray[CYAN_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;

   spriteAttrArray[PINK_GHOST_SPRITENUM].x = 122;
   spriteAttrArray[PINK_GHOST_SPRITENUM].y = 84;
   spriteAttrArray[PINK_GHOST_SPRITENUM].color = LIGHTRED;
   spriteAttrArray[PINK_GHOST_SPRITENUM].prevdir = WEST;
   spriteAttrArray[PINK_GHOST_SPRITENUM].xdir = -2;
   spriteAttrArray[PINK_GHOST_SPRITENUM].ydir = 0;
   spriteAttrArray[PINK_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_2;
   spriteAttrArray[PINK_GHOST_EYES_SPRITENUM].color = WHITE;
   spriteAttrArray[PINK_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;

   spriteAttrArray[BROWN_GHOST_SPRITENUM].x = 114;
   spriteAttrArray[BROWN_GHOST_SPRITENUM].y = 84;
   spriteAttrArray[BROWN_GHOST_SPRITENUM].color = DARKYELLOW;
   spriteAttrArray[BROWN_GHOST_SPRITENUM].prevdir = WEST;
   spriteAttrArray[BROWN_GHOST_SPRITENUM].xdir = -2;
   spriteAttrArray[BROWN_GHOST_SPRITENUM].ydir = 0;
   spriteAttrArray[BROWN_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_2;
   spriteAttrArray[BROWN_GHOST_EYES_SPRITENUM].color = WHITE;
   spriteAttrArray[BROWN_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;
}


/* ******************************************************************************************************************************
   | cycle through all audio ports on the TRS80MXS and shut off all 4 voices per chip                                           |
   ******************************************************************************************************************************
*/
void audioSilence(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static int j;

    for(int i=0x82;i<0x86;++i)
      for(int j=0;j<4;++j)
        setVolume(i,j,0);
}


/* ******************************************************************************************************************************
   | turn off audio, wait a certain amount of time, reset the volume back up ("musical rest")                                   |
   ******************************************************************************************************************************
*/
void rest(unsigned int x) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  audioSilence(); 
  hold(x); 
}


/* ******************************************************************************************************************************
   | Make pacman wakka wakka sound                                                                                              |
   ******************************************************************************************************************************
*/
void wakka(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  if(circ_bbuf_empty(&audio_circ_buffer_x83_0)) {
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_E4);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_A5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, -1);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_B5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_GS4);
  }
}

void bloip(void) {
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_C5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_D5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_E5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_FS5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_GS5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_AS5);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_B6);
}

void fruitEatenSound(void) {
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_C3);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_F3);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_B3);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_C4);
    circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_F4);
}


/* ******************************************************************************************************************************
   | Display one of the fruits                                                                                                  |
   ******************************************************************************************************************************
*/
void displayCherry(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = DARKRED;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_CHERRY;
}
void displayApple(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = DARKRED;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_APPLE;
}
void displayPeach(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = LIGHTRED;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_PEACH;
}
void displayMelon(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = DARKGREEN;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_MELON;
}
void displayStrawberry(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = DARKRED;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_STRAWBERRY;
}
void displayKey(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = GRAY;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_KEY;
}
void displayBell(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = LIGHTYELLOW;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_BELL;
}
void displayGalaxian(void) {
    spriteAttrArray[FRUIT_SPRITENUM].color = LIGHTYELLOW;
    spriteAttrArray[FRUIT_SPRITENUM].patt = PATT_GALAXIAN;
}

void displayFruit(int i) {
      spriteAttrArray[FRUIT_SPRITENUM].x = PACMAN_HOME_X;
      spriteAttrArray[FRUIT_SPRITENUM].y = PACMAN_HOME_Y;

        switch(i) {
            case 1:
                displayCherry();
            break;
            case 2:
                displayStrawberry();
            break;
            case 3:
                displayPeach();
            break;
            case 4:
                displayApple();
            break;
            case 5:
                displayMelon();
            break;
            case 6:
                displayGalaxian();
            break;
            case 7:
                displayBell();
            break;
            case 8:
                displayKey();
            break;

            default:
                displayFruit((getRand256() % 7) + 1);
        }
}

/* ******************************************************************************************************************************
   | movePacman one step and eat dot if one is under him                                                                        |
   ******************************************************************************************************************************
*/
void movePacman(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
    static byte c;
    static int x;
    static int y;
    static int xd;
    static int yd;
    static byte cc;
    static byte xx;
    static byte yy;

    x = spriteAttrArray[PACMAN_SPRITENUM].x;
    y = spriteAttrArray[PACMAN_SPRITENUM].y;
    xd = spriteAttrArray[PACMAN_SPRITENUM].xdir;
    yd = spriteAttrArray[PACMAN_SPRITENUM].ydir;    

    if((xd < 0 && canGoWest(PACMAN_SPRITENUM)) || (xd > 0 && canGoEast(PACMAN_SPRITENUM)))
        spriteAttrArray[PACMAN_SPRITENUM].x = x + xd;
    if((yd < 0 && canGoNorth(PACMAN_SPRITENUM)) || (yd > 0 && canGoSouth(PACMAN_SPRITENUM)))
        spriteAttrArray[PACMAN_SPRITENUM].y = y + yd;

    if(x == 0)
        spriteAttrArray[PACMAN_SPRITENUM].x = 238;
    else
        if(x == 240)
           spriteAttrArray[PACMAN_SPRITENUM].x = 2;
   
    pacmanAnimationPosition = pacmanAnimationPosition + pacmanAnimationDirection;
    if(pacmanAnimationPosition > 1 || pacmanAnimationPosition < 1) 
      pacmanAnimationDirection = -pacmanAnimationDirection;

    if(yd < 0) 
        spriteAttrArray[PACMAN_SPRITENUM].patt = pacmanAnimationPosition + NORTH_PACMAN_PAT_OFFSET;
    else 
        if(yd > 0) 
            spriteAttrArray[PACMAN_SPRITENUM].patt = pacmanAnimationPosition + SOUTH_PACMAN_PAT_OFFSET;
        else
            if(xd < 0) 
                spriteAttrArray[PACMAN_SPRITENUM].patt = pacmanAnimationPosition + WEST_PACMAN_PAT_OFFSET;
            else 
                spriteAttrArray[PACMAN_SPRITENUM].patt = pacmanAnimationPosition + EAST_PACMAN_PAT_OFFSET;

    x = x >> 3;
    y = y >> 3;

    xx = (byte) x + 1;
    yy = (byte) y + 1;

    cc = getCharFromNameTable(xx,yy);

    if(cc == 'r' || cc == ENERGIZER_RIGHT_CHAR) {
      updateScore(10);
      dotCtr++;
      wakka();
      setCharInNameTable(xx,yy,' ');
      if(cc == ENERGIZER_RIGHT_CHAR) {
        circ_bbuf_push(&audio_circ_buffer_x83_0, BANK1_C6);
        energizerCtr = 200;
        setCharInNameTable(xx-1,yy,' ');
      }
    }

    if(energizerCtr > 0)
      energizerCtr--;

    if(y == 21) {
      yy++;
      cc = getCharFromNameTable(xx,yy);
      if(cc == 'q') {
        updateScore(10);
        dotCtr++;
        wakka();
        setCharInNameTable(xx,yy,'s');
      }
    }

    if((gameCtr >> 8) == 2 && fruitEatenOnThisLevel == 0) 
        displayFruit(levelCtr);

    if((gameCtr >> 8) == 4) 
        spriteAttrArray[FRUIT_SPRITENUM].color = TRANSPARENT;
}


/* ******************************************************************************************************************************
   | read joystick or keyboard and set pacman sprite direction                                                                  |
   ******************************************************************************************************************************
*/
void checkControls(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
        static byte oldk;
        static byte k;


        #ifdef GCC_COMPILED
            k = getJoystick(LEFT_POS);
        #else
            k = getFastLeftJoystick();
        #endif

        if(k == J_E && canGoEast(PACMAN_SPRITENUM)) {
          spriteAttrArray[PACMAN_SPRITENUM].xdir=2;
          spriteAttrArray[PACMAN_SPRITENUM].ydir=0;
        }
        else
        if(k == J_N && canGoNorth(PACMAN_SPRITENUM)) {
          spriteAttrArray[PACMAN_SPRITENUM].ydir=-2;
          spriteAttrArray[PACMAN_SPRITENUM].xdir=0;
        }
        else
        if(k == J_W && canGoWest(PACMAN_SPRITENUM)) {
          spriteAttrArray[PACMAN_SPRITENUM].xdir=-2;
          spriteAttrArray[PACMAN_SPRITENUM].ydir=0;
        }
        else
        if(k == J_S && canGoSouth(PACMAN_SPRITENUM)) {
          spriteAttrArray[PACMAN_SPRITENUM].ydir=2;
          spriteAttrArray[PACMAN_SPRITENUM].xdir=0;
        }

        #ifdef GCC_COMPILED
            k = getJoystick(RIGHT_POS);
        #else
            k = getFastRightJoystick();
        #endif

        if(k == J_BUTTON && oldk != J_BUTTON) { // switch ghosts with the hat
          if(GhostWithHat == RED_GHOST_SPRITENUM)
            GhostWithHat = CYAN_GHOST_SPRITENUM;
          else
          if(GhostWithHat == CYAN_GHOST_SPRITENUM )
            GhostWithHat = PINK_GHOST_SPRITENUM;
          else
          if(GhostWithHat == PINK_GHOST_SPRITENUM)
            GhostWithHat = BROWN_GHOST_SPRITENUM;
          else
            GhostWithHat = RED_GHOST_SPRITENUM;
        }
        oldk = k;

         if(k == J_E && canGoEast(GhostWithHat)) 
              goEast(GhostWithHat);
         else
         if(k == J_N && canGoNorth(GhostWithHat)) 
              goNorth(GhostWithHat);
         else
         if(k == J_W && canGoWest(GhostWithHat)) 
              goWest(GhostWithHat);
         else
         if(k == J_S && canGoSouth(GhostWithHat)) 
              goSouth(GhostWithHat);
}

/* ******************************************************************************************************************************
   | play introduction music                                                                                                    |
   ******************************************************************************************************************************
*/
void introMusic(void)
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  play(AUDIOCHIP0,0,BANK1_C3,15);        play(AUDIOCHIP0,1,BANK1_C5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_C6,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C4,15);        play(AUDIOCHIP0,1,BANK1_G5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_E5,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C3,15);        play(AUDIOCHIP0,1,BANK1_C6,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_G5,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C4,15);        play(AUDIOCHIP0,1,BANK1_E5,15);               hold(EIGHTHNOTE);
                                                                                       hold(EIGHTHNOTE);

  play(AUDIOCHIP0,0,BANK1_CS3,15);       play(AUDIOCHIP0,1,BANK1_CS5,15);              hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_CS6,15);              hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_CS4,15);       play(AUDIOCHIP0,1,BANK1_GS5,15);              hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_F5,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_CS3,15);       play(AUDIOCHIP0,1,BANK1_CS6,15);              hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_GS5,15);              hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_CS4,15);       play(AUDIOCHIP0,1,BANK1_F5,15);               hold(EIGHTHNOTE);
                                                                                       hold(EIGHTHNOTE);

  play(AUDIOCHIP0,0,BANK1_C3,15);        play(AUDIOCHIP0,1,BANK1_C5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_C6,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C4,15);        play(AUDIOCHIP0,1,BANK1_G5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_E5,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C3,15);        play(AUDIOCHIP0,1,BANK1_C6,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_G5,15);               hold(EIGHTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C4,15);        play(AUDIOCHIP0,1,BANK1_E5,15);               hold(EIGHTHNOTE);
                                                                                       hold(EIGHTHNOTE);

  play(AUDIOCHIP0,0,BANK1_G3,15);        play(AUDIOCHIP0,1,BANK1_F5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_FS5,15);              hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_G5,15);               hold(SIXTEENTHNOTE);
                                                                                       rest(TWOFIFTYSIXTHNOTE); 
  play(AUDIOCHIP0,0,BANK1_A4,15);        play(AUDIOCHIP0,1,BANK1_G5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_GS5,15);              hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_A5,15);               hold(SIXTEENTHNOTE);
                                                                                       rest(TWOFIFTYSIXTHNOTE);
  play(AUDIOCHIP0,0,BANK1_B4,15);        play(AUDIOCHIP0,1,BANK1_A5,15);               hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_AS5,15);              hold(EIGHTHNOTE);
                                         play(AUDIOCHIP0,1,BANK1_B5,15);               hold(SIXTEENTHNOTE);
                                                                                       rest(TWOFIFTYSIXTHNOTE);
  play(AUDIOCHIP0,0,BANK1_C4,15);        play(AUDIOCHIP0,1,BANK1_C6,15);               hold(EIGHTHNOTE);
                                                                                       hold(EIGHTHNOTE);
                                                                                       hold(EIGHTHNOTE);
                                                                                       hold(EIGHTHNOTE);
  audioSilence();
}

/* ******************************************************************************************************************************
   | resets the maze back to normal                                                                                             |
   ******************************************************************************************************************************
*/
void resetMap(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
      spriteAttrArray[RED_GHOST_SPRITENUM].isEyes = FALSE;
      spriteAttrArray[CYAN_GHOST_SPRITENUM].isEyes = FALSE;
      spriteAttrArray[BROWN_GHOST_SPRITENUM].isEyes = FALSE;
      spriteAttrArray[PINK_GHOST_SPRITENUM].isEyes = FALSE;

      energizerCtr = 0;
      putGhostsInBox();
      ghostsNormal();  
      spriteAttrArray[PACMAN_SPRITENUM].x = PACMAN_HOME_X;
      spriteAttrArray[PACMAN_SPRITENUM].y = PACMAN_HOME_Y;
      spriteAttrArray[FRUIT_SPRITENUM].x = PACMAN_HOME_X;
      spriteAttrArray[FRUIT_SPRITENUM].y = PACMAN_HOME_Y;
      spriteAttrArray[FRUIT_SPRITENUM].color = TRANSPARENT;
      gameCtr = 0;
      fruitEatenOnThisLevel = 0;
}


/* ******************************************************************************************************************************
   | decide if all 260 dots have been eaten, if so, flash the maze                                                              |
   ******************************************************************************************************************************
*/
void checkForAllDotsGone(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
      static char cc;
      if(dotCtr == 260) {
        audioSilence();
        for(cc = 0; cc < 6; cc++) {
          hold(1000);
          setEverythingWhite();
          #ifdef GCC_COMPILED
              updateEmulatedVDPScreen();
              SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, texture, NULL, NULL);
              SDL_RenderPresent(renderer);
          #endif
          hold(1000);
          setMazeColors();
          #ifdef GCC_COMPILED
              updateEmulatedVDPScreen();
              SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
              SDL_RenderClear(renderer);
              SDL_RenderCopy(renderer, texture, NULL, NULL);
              SDL_RenderPresent(renderer);
          #endif
        }
      dotCtr = 0;
      ++levelCtr;
      drawDots();
      resetMap();
    }
}

/* ******************************************************************************************************************************
   | dead pac-man animation, and map reset                                                                                      |
   ******************************************************************************************************************************
*/
void pacmanDead(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static int i;
  static int notes[] = {
    BANK1_F5,  BANK1_E5, BANK1_DS5, BANK1_D5,
    BANK1_DS5, BANK1_D5, BANK1_CS5, BANK1_C5,
    BANK1_CS5, BANK1_F5, BANK1_F6,  BANK1_F6
  };

  spriteAttrArray[PACMAN_SPRITENUM].xdir = 0;
  spriteAttrArray[PACMAN_SPRITENUM].ydir = 0;
  audioSilence();

    for(i=PATT_PACMAN_DYING_START;i<=PATT_PACMAN_DYING_END;++i) {
      hold(500);
      play(AUDIOCHIP0,1,notes[i-PATT_PACMAN_DYING_START],15);
      spriteAttrArray[PACMAN_SPRITENUM].patt = i;
      syncSpriteAttrStructsToHardware();
      #ifdef GCC_COMPILED
          updateEmulatedVDPScreen();
          SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
          SDL_RenderClear(renderer);
          SDL_RenderCopy(renderer, texture, NULL, NULL);
          SDL_RenderPresent(renderer);
      #endif
    }

    resetMap();
}


/* ******************************************************************************************************************************
   | animation for when a ghost gets eaten                                                                                      |
   ******************************************************************************************************************************
*/
void ghostEaten(int i) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
      spriteAttrArray[i].color = TRANSPARENT;
      spriteAttrArray[i+4].color = WHITE;
      spriteAttrArray[i].isEyes = TRUE;
      ghostCtr = ghostCtr + 1;
      popScoreGameCtr = gameCtr;

      spriteAttrArray[POP_SCORE_SPRITENUM].color = CYAN;
      spriteAttrArray[POP_SCORE_SPRITENUM].x = spriteAttrArray[i].x;
      spriteAttrArray[POP_SCORE_SPRITENUM].y = spriteAttrArray[i].y;

      if(ghostCtr == 1) {
          spriteAttrArray[POP_SCORE_SPRITENUM].patt = PATT_200;
          updateScore(200);
      }
      else
        if(ghostCtr == 2) {
            spriteAttrArray[POP_SCORE_SPRITENUM].patt = PATT_400;
            updateScore(400);
        }
        else
            if(ghostCtr == 3) {
                spriteAttrArray[POP_SCORE_SPRITENUM].patt = PATT_800; 
                updateScore(800); 
            }
            else
                if(ghostCtr == 4) {
                    spriteAttrArray[POP_SCORE_SPRITENUM].patt = PATT_1600;
                    updateScore(1600);
                }
                else {
                    spriteAttrArray[POP_SCORE_SPRITENUM].patt = PATT_QUESTIONMARKS;
                    updateScore(3200);
                }

      bloip();
}
/* ******************************************************************************************************************************
   | check collision between pacman and ghosts                                                                                  |
   ******************************************************************************************************************************
*/
void checkCollisions(void)
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static int px;
  static int py;
  static int pxl;
  static int pyl;
  static int i;
  static int gx;
  static int gy;
  static int gxl;
  static int gyl;

  px = spriteAttrArray[PACMAN_SPRITENUM].x+2;
  py = spriteAttrArray[PACMAN_SPRITENUM].y+2;
  pxl = px + 14;
  pyl = py + 14;

  for(i=RED_GHOST_SPRITENUM;i<=BROWN_GHOST_SPRITENUM;++i) {
    gx = spriteAttrArray[i].x+2;
    gy = spriteAttrArray[i].y+2;
    gxl = spriteAttrArray[i].x+14;
    gyl = spriteAttrArray[i].y+14;
    if (spriteAttrArray[i].isEyes == FALSE && px  < gxl && pxl > gx && py  < gyl && pyl > gy) {
      if(energizerCtr == 0) {
          pacmanDead();
          lives--;
          drawPacmanLives();
          if(lives < 0)
            gameLoopRunning = FALSE;
          i = BROWN_GHOST_SPRITENUM+1; // not fair to die more than once
      }
      else {
          ghostEaten(i);
          i = BROWN_GHOST_SPRITENUM+1; // get out if we've had a collision
      }    
    }      
  }

  if(spriteAttrArray[FRUIT_SPRITENUM].color != TRANSPARENT) {
        gx = spriteAttrArray[FRUIT_SPRITENUM].x+2;
        gy = spriteAttrArray[FRUIT_SPRITENUM].y+2;
        gxl = spriteAttrArray[FRUIT_SPRITENUM].x+14;
        gyl = spriteAttrArray[FRUIT_SPRITENUM].y+14;    
        if(px  < gxl && pxl > gx && py  < gyl && pyl > gy) {
            updateScore(1000 * levelCtr);
            fruitEatenSound();
            fruitEatenOnThisLevel = 1;
            spriteAttrArray[FRUIT_SPRITENUM].color = TRANSPARENT;
        }
  }
}


/* ******************************************************************************************************************************
   | clear maze screen and turn sprites off                                                                                     |
   ******************************************************************************************************************************
*/
void clearMazeShutOffSprites(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
  static byte k;
  static byte j;

   for(k=0;k<24;++k)
     for(j=0;j<32;++j)
        setCharInNameTable(j,k,' ');

   for(j=0;j<=MAX_SPRITENUM;++j) {
    spriteAttrArray[j].color = TRANSPARENT;
   }
   syncSpriteAttrStructsToHardware();
   forceAllSpritesTransparent();
}


/* ******************************************************************************************************************************
   | Game attract screen                                                                                                        |
   ******************************************************************************************************************************
*/
void attractScreen() {
    int x = 0;
    int y = 4;
    byte j;
    dotCtr = 0;
    lives = 3;
    levelCtr = 1;

    for(int i=0;i<32;++i)
      setColorTableEntry(i,DARKBLUE,BLACK);

    for(int i=12;i<=15;++i)
      setColorTableEntry(i,DARKRED,BLACK);

    setColorTableEntry(5,DARKYELLOW,BLACK);

    setCharsInNameTable(0,23,"dbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbf");
    setCharsInNameTable(0,0 , "dbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbf");
    for(int i=1;i<23;++i) {
      setCharInNameTable(0, i,'a');
      setCharInNameTable(31,i,'a');
    }

    setCharsInNameTable(4+x,y,   ",  , +,(");
    setCharsInNameTable(4+x,y+1, ",  , )( ");
    setCharsInNameTable(4+x,y+2, ")(+*  )(");
    setCharsInNameTable(4+x,y+3, " )*  ),*");

    setCharsInNameTable(4+x,y+5, ",,( +,( +,( ,(+, +,( , ,");
    setCharsInNameTable(4+x,y+6, ", , , , , , ,)*, , , ,(,");
    setCharsInNameTable(4+x,y+7, ",,* ,,, ,   ,  , ,,, ,),");
    setCharsInNameTable(4+x,y+8, ",   , , , , ,  , , , , ,");
    setCharsInNameTable(4+x,y+9, ",   , , ),* ,  , , , , ,");

    setCharsInNameTable(x+1,y+15," PRESS JOYSTICK FIRE TO BEGIN");
    setCharsInNameTable(x+1,y+16,"       JOYSTICK DOWN TO QUIT");
    setCharsInNameTable(x+1,y+18,"    @ 2020 JOE ROUNCEVILLE");

    x = 0;
    y = 120;
    while(getJoystick(LEFT_POS) != J_BUTTON) {
       hold(100);

       if(getJoystick(LEFT_POS) == J_S) {
          audioSilence();
          clearMazeShutOffSprites();
          #ifdef GCC_COMPILED
              SDLShutdown();
          #endif
          exit(0);
       }

       spriteAttrArray[PACMAN_SPRITENUM].x = x;
       spriteAttrArray[PACMAN_SPRITENUM].y = y;
       spriteAttrArray[PACMAN_SPRITENUM].color = DARKYELLOW;

       spriteAttrArray[RED_GHOST_SPRITENUM].x = x + 30;
       spriteAttrArray[RED_GHOST_SPRITENUM].y = y;
       spriteAttrArray[RED_GHOST_SPRITENUM].color = DARKBLUE;
       spriteAttrArray[RED_GHOST_SPRITENUM].patt = PATT_SCARED_GHOST;

       spriteAttrArray[HAT_SPRITENUM].x = x + 30;
       spriteAttrArray[HAT_SPRITENUM].y = y - 4;
       spriteAttrArray[HAT_SPRITENUM].color = DARKGREEN;
       spriteAttrArray[HAT_SPRITENUM].patt = PATT_HAT;

       syncSpriteAttrStructsToHardware();

       #ifdef GCC_COMPILED
           updateEmulatedVDPScreen();
           SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
           SDL_RenderClear(renderer);
           SDL_RenderCopy(renderer, texture, NULL, NULL);
           SDL_RenderPresent(renderer);
       #endif
       x = x + 2;
       spriteAttrArray[PACMAN_SPRITENUM].patt = PATT_PACMAN_E1 + ((x/2) % 3);

       if(x>255) x = 0;
    };

    clearMazeShutOffSprites();
}


/* ******************************************************************************************************************************
   | Initializes global variables that set up graphics state, clears out sprite memory, and cleans off screen buffer            |
   ******************************************************************************************************************************
*/
void clearScreenAndInitializeSprites(void) {
   static int i;
   g.graphicsMode = GRAPHICSMODE1;
   g.externalVideoEnabled = FALSE;
   g.interruptsEnabled = FALSE;
   g.spritesMagnified = FALSE;
   g.sprites16x16Enabled = TRUE;
   g.textForegroundColor = WHITE;
   g.backgroundColor = BLACK;
   setGraphicsMode();

   memset(spriteAttrArray,0x0,sizeof(spriteAttrArray));
   for(i=0;i<32;++i)
    spriteAttrArray[i].patt = -1;

   spriteAttrArray[PACMAN_SPRITENUM].x = 120;
   spriteAttrArray[PACMAN_SPRITENUM].y = 128;
   spriteAttrArray[PACMAN_SPRITENUM].patt = pacmanAnimationPosition;
   
   clearMazeShutOffSprites();
}


/* ******************************************************************************************************************************
   | game setup stuff...                                                                                                        |
   ******************************************************************************************************************************
*/
void setupGame(void) 
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{
   static int i;
   score = 0;
   bgMusicDirection = (int)fmax((double)abs(BANK1_GS5 - BANK1_DS5) / 6,(double)1); // wonky math necessary so direction works for both gcc and zcc

   audioSilence();

   srand(getRand256());

   setPatterns();

   clearScreenAndInitializeSprites();

   static char pacman_e1[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_e2[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x78,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xE0,0x00,0x00,0x00,0xE0,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_e3[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7E,0x7C,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x00};
   static char pacman_n1[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_n2[] = {0x00,0x00,0x00,0x18,0x38,0x3C,0x7C,0x7C,0x7E,0x7E,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0x00,0x30,0x38,0x78,0x7C,0x7C,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_n3[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x70,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x1C,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_w1[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_w2[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x0F,0x01,0x00,0x01,0x0F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0x3C,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_w3[] = {0x00,0x00,0x07,0x07,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0x7C,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_s1[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_s2[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7E,0x7E,0x7E,0x7C,0x7C,0x3C,0x38,0x18,0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0x7C,0x7C,0x78,0x38,0x30,0x00,0x00};
   static char pacman_s3[] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7E,0x7C,0x78,0x70,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0x7C,0x3C,0x1C,0x0C,0x00,0x00,0x00,0x00,0x00};

   setSpritePatternByArray(PATT_PACMAN_E1, pacman_e1,32);
   setSpritePatternByArray(PATT_PACMAN_E2, pacman_e2,32);
   setSpritePatternByArray(PATT_PACMAN_E3, pacman_e3,32);
   setSpritePatternByArray(PATT_PACMAN_W1, pacman_w1,32);
   setSpritePatternByArray(PATT_PACMAN_W2, pacman_w2,32);
   setSpritePatternByArray(PATT_PACMAN_W3, pacman_w3,32);
   setSpritePatternByArray(PATT_PACMAN_N1, pacman_n1,32);
   setSpritePatternByArray(PATT_PACMAN_N2, pacman_n2,32);
   setSpritePatternByArray(PATT_PACMAN_N3, pacman_n3,32);
   setSpritePatternByArray(PATT_PACMAN_S1, pacman_s1,32);
   setSpritePatternByArray(PATT_PACMAN_S2, pacman_s2,32);
   setSpritePatternByArray(PATT_PACMAN_S3, pacman_s3,32);

   static char normal_ghost_1[] =      {0x00,0x03,0x0F,0x1F,0x33,0x21,0x21,0x21,0x73,0x7F,0x7F,0x7F,0x7F,0x7F,0x6C,0x44,0x00,0x80,0xE0,0xF0,0x98,0x08,0x08,0x08,0x9C,0xFC,0xFC,0xFC,0xFC,0xFC,0xEC,0xC4};
   static char normal_ghost_2[] =      {0x00,0x03,0x0F,0x1F,0x33,0x21,0x21,0x21,0x73,0x7F,0x7F,0x7F,0x7F,0x7F,0x7B,0x31,0x00,0x80,0xE0,0xF0,0x98,0x08,0x08,0x08,0x9C,0xFC,0xFC,0xFC,0xFC,0xFC,0xDC,0x88};
   static char normal_ghost_eyes_n[] = {0x00,0x00,0x00,0x00,0x00,0x12,0x1E,0x1E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0xF0,0xF0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char normal_ghost_eyes_s[] = {0x00,0x00,0x00,0x00,0x0C,0x1E,0x1E,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xF0,0xF0,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char normal_ghost_eyes_e[] = {0x00,0x00,0x00,0x00,0x0C,0x1E,0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xF0,0xC0,0xC0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char normal_ghost_eyes_w[] = {0x00,0x00,0x00,0x00,0x0C,0x1E,0x06,0x06,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xF0,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char scared_ghost[] =        {0x00,0x03,0x0F,0x1F,0x3F,0x3F,0x3F,0x79,0x79,0x7F,0x7F,0x66,0x59,0x7F,0x6E,0x46,0x00,0xC0,0xF0,0xF8,0xFC,0xFC,0xFC,0x9E,0x9E,0xFE,0xFE,0x66,0x9A,0xFE,0x76,0x62};

   setSpritePatternByArray(PATT_NORMAL_GHOST_1, normal_ghost_1,32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_2, normal_ghost_2,32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_N, normal_ghost_eyes_n, 32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_S, normal_ghost_eyes_s, 32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_E, normal_ghost_eyes_e, 32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_W, normal_ghost_eyes_w, 32);
   setSpritePatternByArray(PATT_SCARED_GHOST, scared_ghost, 32);

   static char hat[] = {0x00,0x04,0x0E,0x0F,0x0F,0x9F,0xFF,0x7F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x70,0xF0,0xF0,0xF9,0xFF,0xFE,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   setSpritePatternByArray(PATT_HAT,hat,32);

   attractScreen();

   clearScreenAndInitializeSprites();

   putGhostsInBox();

   drawMaze();

   static char pacman_dying_start[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x70,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x1C,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_dying_1[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_dying_2[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x7C,0x7F,0x3F,0x1F,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x7C,0xFC,0xF8,0xF0,0xC0,0x00};
   static char pacman_dying_3[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x7F,0x3F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0xFC,0xF8,0xE0,0x00};
   static char pacman_dying_4[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x7F,0x3F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0xF8,0xE0};
   static char pacman_dying_5[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0F,0x3F,0x7F,0x3E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF8,0xFC,0xF8,0x60};
   static char pacman_dying_6[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x0F,0x1F,0x7F,0x7E,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xFC,0xFC,0x78};
   static char pacman_dying_7[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x0F,0x1F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xE0,0xF0,0xE0};
   static char pacman_dying_8[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x03,0x07,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0xC0,0x80};
   static char pacman_dying_9[]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char pacman_dying_end[]   = {0x00,0x00,0x00,0x04,0x02,0x10,0x08,0x00,0x00,0x30,0x00,0x00,0x08,0x10,0x02,0x04,0x00,0x00,0x00,0x20,0x40,0x08,0x10,0x00,0x00,0x0C,0x00,0x00,0x10,0x08,0x40,0x20};

   setSpritePatternByArray(PATT_PACMAN_DYING_START, pacman_dying_start,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_1, pacman_dying_1,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_2, pacman_dying_2,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_3, pacman_dying_3,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_4, pacman_dying_4,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_5, pacman_dying_5,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_6, pacman_dying_6,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_7, pacman_dying_7,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_8, pacman_dying_8,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_9, pacman_dying_9,32);
   setSpritePatternByArray(PATT_PACMAN_DYING_END, pacman_dying_end,32);

   static char TwoHundred[] =     {0x00,0x00,0x00,0x00,0x31,0x4A,0x0A,0x0A,0x32,0x42,0x42,0x79,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0x52,0x52,0x52,0x52,0x52,0x52,0x8C,0x00,0x00,0x00,0x00};
   static char FourHundred[] =    {0x00,0x00,0x00,0x00,0x09,0x1A,0x2A,0x4A,0x7A,0x0A,0x0A,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0x52,0x52,0x52,0x52,0x52,0x52,0x8C,0x00,0x00,0x00,0x00};
   static char EightHundred[] =   {0x00,0x00,0x00,0x00,0x31,0x4A,0x4A,0x32,0x4A,0x4A,0x4A,0x31,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0x52,0x52,0x52,0x52,0x52,0x52,0x8C,0x00,0x00,0x00,0x00};
   static char SixteenHundred[] = {0x00,0x00,0x00,0x00,0x88,0x91,0xA1,0xA1,0xB9,0xA5,0xA5,0x98,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x29,0x29,0x29,0x29,0x29,0x29,0xC6,0x00,0x00,0x00,0x00};
   static char QuestionMarks[] =  {0x00,0x00,0x00,0x31,0x4A,0x08,0x08,0x10,0x21,0x21,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8C,0x52,0x42,0x42,0x84,0x08,0x08,0x00,0x08,0x00,0x00,0x00,0x00};

   setSpritePatternByArray(PATT_200,           TwoHundred,     32);
   setSpritePatternByArray(PATT_400,           FourHundred,    32);
   setSpritePatternByArray(PATT_800,           EightHundred,   32);
   setSpritePatternByArray(PATT_1600,          SixteenHundred, 32);
   setSpritePatternByArray(PATT_QUESTIONMARKS, QuestionMarks,  32);
   
   static char Cherry[] =         {0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x06,0x1C,0x3E,0x2E,0x2E,0x26,0x1C,0x00,0x00,0x00,0x20,0x60,0xC0,0x80,0x00,0x80,0xC0,0x78,0x7C,0x7C,0x5C,0x4C,0x38,0x00,0x00};
   static char Apple[] =          {0x00,0x00,0x00,0x1D,0x3E,0x3F,0x3F,0x3F,0x3F,0x3F,0x1F,0x1F,0x0F,0x06,0x00,0x00,0x00,0x00,0x80,0x70,0xF8,0xFC,0xFC,0xFC,0xEC,0xEC,0xD8,0xF8,0xF0,0xE0,0x00,0x00};
   static char Peach[] =          {0x00,0x00,0x00,0x01,0x01,0x0B,0x1D,0x3E,0x3F,0x3F,0x3F,0x1F,0x1F,0x07,0x00,0x00,0x00,0x40,0xF0,0x60,0x00,0xB0,0x78,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xE0,0x00,0x00};
   static char Strawberry[] =     {0x00,0x00,0x01,0x07,0x1B,0x2C,0x3B,0x3F,0x36,0x1F,0x1F,0x0D,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0xC0,0xB0,0x68,0xF8,0xB8,0xE8,0xF0,0xB0,0xE0,0xC0,0x00,0x00,0x00};
   static char Bell[] =           {0x00,0x00,0x03,0x0C,0x1B,0x17,0x37,0x3F,0x6F,0x6F,0x7F,0x61,0x3F,0x01,0x00,0x00,0x00,0x00,0x00,0xC0,0xE0,0xE0,0xF0,0xF0,0xF8,0xF8,0xF8,0x98,0xF0,0x80,0x00,0x00};
   static char Key[] =            {0x00,0x00,0x03,0x0C,0x0F,0x0F,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x00,0x00,0x00,0x00,0x80,0x60,0xE0,0xE0,0x80,0xC0,0x80,0x40,0x80,0xC0,0x80,0x00,0x00,0x00};
   static char Melon[] =          {0x00,0x08,0x07,0x01,0x02,0x0F,0x1B,0x1F,0x2F,0x3D,0x2F,0x1F,0x1B,0x0F,0x03,0x00,0x00,0x00,0x80,0x00,0x80,0xE0,0xF0,0x70,0xF8,0xD8,0xF8,0x70,0xF0,0xE0,0x80,0x00};
   static char Galaxian[] =       {0x00,0x00,0x01,0x23,0x27,0x3C,0x3E,0x2F,0x37,0x1B,0x0D,0x05,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x88,0xC8,0x78,0xF8,0xE8,0xD8,0xB0,0x60,0x40,0x00,0x00,0x00,0x00};

   setSpritePatternByArray(PATT_CHERRY,           Cherry,     32);
   setSpritePatternByArray(PATT_APPLE,            Apple,      32);
   setSpritePatternByArray(PATT_PEACH,            Peach,      32);
   setSpritePatternByArray(PATT_STRAWBERRY,       Strawberry, 32);
   setSpritePatternByArray(PATT_BELL,             Bell,       32);
   setSpritePatternByArray(PATT_KEY,              Key,        32);
   setSpritePatternByArray(PATT_MELON,            Melon,      32);
   setSpritePatternByArray(PATT_GALAXIAN,         Galaxian,   32);

   spriteAttrArray[PACMAN_SPRITENUM].color = DARKYELLOW;

   moveGhosts(); // force the ghosts eyes to appear

   #ifdef GCC_COMPILED
       updateEmulatedVDPScreen();
       SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
       SDL_RenderClear(renderer);
       SDL_RenderCopy(renderer, texture, NULL, NULL);
       SDL_RenderPresent(renderer);
   #endif

   spriteAttrArray[HAT_SPRITENUM].x = 0; // initialize the hat so it's invisible
   spriteAttrArray[HAT_SPRITENUM].y = 0;
   spriteAttrArray[HAT_SPRITENUM].patt = PATT_HAT;
   spriteAttrArray[HAT_SPRITENUM].color = TRANSPARENT;

   introMusic(); 
} 



/* ******************************************************************************************************************************
   | This is the main audio routine.  It pulls data off the audio circular buffer (like munching sounds) and plays them         |
   | It also makes the siren sound in the background that is so iconic to pac-man.                                              |
   | This routine gets kicked once per game loop.                                                                               |
   ******************************************************************************************************************************
*/
void audioBufferProcess(void)  
#ifndef GCC_COMPILED 
    __z88dk_fastcall
#endif
{ 
  static int x = BANK1_GS5;
  static int y = 0;
  static byte quiet = 0;
  static int circ_buf_data;

  // if there's something sitting on the circular audio buffer, play it, otherwise shut down this audio channel
  if (circ_bbuf_pop(&audio_circ_buffer_x83_0, &circ_buf_data)) {                 
      if(quiet == 0) {                                                         
          setVolume(AUDIOCHIP0,0,0);                                                    
          quiet = 1;                                                             
      }                                                                        
  }                                                                              
  else {                                                                          
      if(circ_buf_data < 0) {                                                  
          setVolume(AUDIOCHIP0,0,0);                                                 
          quiet = 1;                                                           
      }                                                                        
      else {                                                                   
          play(AUDIOCHIP0,0,circ_buf_data,15);                                       
          quiet = 0;                                                           
      }                                                                         
  }

  if(gameCtr & 0x0001) {
      if(energizerCtr == 0) {
        // siren sound in the background
        x = x + bgMusicDirection;

        if(x > BANK1_GS5)
          bgMusicDirection = -bgMusicDirection;
        if(x < BANK1_DS5)
          bgMusicDirection = -bgMusicDirection;
        play(AUDIOCHIP0,1,x,9);
      }
      else {
        // sound that plays in the background when energizer has been eaten
        y = y + 3;
        if(y > 15) y = 0;
        play(AUDIOCHIP0,1,BANK1_C4,y);
      }
  }
} 



#define blinkEnergizers() if(gameCtr & 0x0004) setColorTableEntry(16,BLACK,BLACK);  else  setColorTableEntry(16,WHITE,BLACK);

#ifndef GCC_COMPILED
    #define charat(x,y,c)  memset(0x3c00+((y<<6) + x), c, 1);
    void strat(byte x, byte y, char* s) {
        for(uint i = 0; i < strlen(s); ++i) {
            charat(x+i,y,*(s+i));
        }
    }
#endif


void displayTRSScreen(void) {
    char ch[65];

   #ifdef GCC_COMPILED
       printf(".---------------------------------------.\n");
       printf("(                PACMAN!!               )\n");
       printf("(           YYYY-MM-DD-HH-MM-SS         )\n");
       printf("(              CKSUM               )\n");
       printf("'---------------------------------------'\n\n");
       printf("THIS IS THE SDL VERSION OF VS PACMAN.\n");
   #else
        strcpy(ch,"        XXXXXXXXXXXXX       ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 0,ch);
        strcpy(ch,"    XXXXXXXXXXXXXXXXXXXXX   ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 1,ch);
        strcpy(ch,"  XXXXXXXXXXXXXXXXXXXXXXXXX ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 2,ch);
        strcpy(ch,"  XXXXXXXXXXXXXXXXXXXXXXXXX ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 3,ch);
        strcpy(ch,"XXXXXXXXXXXXXXXXXXXXXXX     ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 4,ch);
        strcpy(ch,"XXXXXXXXXXXXXXXXX           ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 5,ch);
        strcpy(ch,"XXXXXXXXXXX                 ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 6,ch);
        strcpy(ch,"XXXXXXXXXXXXXXXXX           ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 7,ch);
        strcpy(ch,"XXXXXXXXXXXXXXXXXXXXXXX     ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 8,ch);
        strcpy(ch,"  XXXXXXXXXXXXXXXXXXXXXXXXX ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29, 9,ch);
        strcpy(ch,"  XXXXXXXXXXXXXXXXXXXXXXXXX ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29,10,ch);
        strcpy(ch,"    XXXXXXXXXXXXXXXXXXXXX   ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29,11,ch);
        strcpy(ch,"        XXXXXXXXXXXXX       ");for(int i=0;i<strlen(ch);++i) {if(ch[i]=='X') {ch[i]=0xbf;}} strat(29,12,ch);
        strat(28,14,"VS PACMAN");
        strat(10,15,"BUILD INFO: YYYY-MM-DD-HH-MM-SS, CKSUM");
        strat( 7, 1,"THIS PROGRAM ");
        strat( 7, 2,"REQUIRES THE");
        strat( 7, 3,"TRS80MXS");
        strat( 7, 4,"GRAPHICS AND");
        strat( 7, 5,"SOUND CARD V2.0+");
        strat( 7, 7,"MAKE SURE YOUR");
        strat( 7, 8,"TRS80MXS AUDIO");
        strat( 7, 9,"AND VIDEO ");
        strat( 7,10,"CABLES ARE ");
        strat( 7,11,"PLUGGED IN TO");
        strat( 7,12,"YOUR MONITOR.");
   #endif
}


/* ******************************************************************************************************************************
   | main code                                                                                                                  |
   ******************************************************************************************************************************
*/
int main(void)
{
   #ifdef GCC_COMPILED
      SDLSetup();
   #endif

   clearTRSScreen();
   displayTRSScreen();

top:
   setupGame();

   // main game loop
   gameLoopRunning = TRUE;
   while(gameLoopRunning == TRUE) {
        gameCtr++;

        audioBufferProcess();
        syncSpriteAttrStructsToHardware();
        checkControls();
        movePacman();
        checkForAllDotsGone();
        moveGhosts();
        checkCollisions();
        blinkEnergizers();

        #ifdef GCC_COMPILED
            emulateTMS9118HardwareUpdate();       
        #endif
   }

   clearMazeShutOffSprites();
   setCharsInNameTable(12,11,"GAME OVER");
   audioSilence();
   
   #ifdef GCC_COMPILED
      emulateTMS9118HardwareUpdate();
   #endif
   hold(10000);

   goto top;

   #ifdef GCC_COMPILED
      SDLShutdown();
   #endif
   clearTRSScreen();
   exit(0);
}