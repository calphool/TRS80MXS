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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notedata.c"

#ifdef __APPLE__
// compiled under GCC-8 rather than ZCC
#define GCC_COMPILED
#endif

#ifdef GCC_COMPILED
    #include <SDL2/SDL.h>
    #define SCREEN_WIDTH 512
    #define SCREEN_HEIGHT 384
#endif

// port addresses
#define LEFT_POS 0x82
#define RIGHT_POS 0x83
#define LEFT2_POS 0x84
#define RIGHT2_POS 0x85

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
#define J_N 239
#define J_NE 111
#define J_E 127
#define J_SE 95
#define J_S 223
#define J_SW 159
#define J_W 191
#define J_NW 175
#define J_BUTTON 247


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




// sprite identifiers
#define PACMAN_SPRITENUM 0

#define RED_GHOST_EYES_SPRITENUM 1
#define CYAN_GHOST_EYES_SPRITENUM 2
#define PINK_GHOST_EYES_SPRITENUM 3
#define BROWN_GHOST_EYES_SPRITENUM 4

#define RED_GHOST_SPRITENUM 5
#define CYAN_GHOST_SPRITENUM 6
#define PINK_GHOST_SPRITENUM 7
#define BROWN_GHOST_SPRITENUM 8

#define MAX_SPRITENUM 8

#define EAST_PACMAN_PAT_OFFSET 0
#define NORTH_PACMAN_PAT_OFFSET 3 
#define WEST_PACMAN_PAT_OFFSET 6
#define SOUTH_PACMAN_PAT_OFFSET 9


/* 0x82 = SOUND 1 when OUT */
/* 0x82 = JOY 1 when IN */
/* 0x83 = SOUND 2 when OUT */
/* 0x83 = JOY 2 when IN */
#ifndef GCC_COMPILED
__sfr __at 0x80 PORTX80;
__sfr __at 0x81 PORTX81;
__sfr __at 0x82 PORTX82;
__sfr __at 0x83 PORTX83;
__sfr __at 0x84 PORTX84;
__sfr __at 0x85 PORTX85;
#else
   char screen_buffer[65535];
   char VDPRegisters[8];
   SDL_Window* window = NULL;
   SDL_Surface* screenSurface = NULL;
   SDL_Renderer* renderer = NULL;
   SDL_Texture * texture = NULL;
   Uint32 * pixels = NULL;
#endif

// graphicsSetup constants
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


// video setup structure
typedef struct {
    unsigned char graphicsMode;
    unsigned char externalVideoEnabled;
    unsigned char interruptsEnabled;
    unsigned char sprites16x16Enabled;
    unsigned char spritesMagnified;
    unsigned char textForegroundColor;
    unsigned char backgroundColor;
    unsigned int NameTableAddr;
    unsigned int ColorTableAddr;
    unsigned int PatternTableAddr;
    unsigned int SpriteAttrTableAddr;
    unsigned int SpritePatternTableAddr;
} graphicsSetup;


//sprite attribute structure
typedef struct {
  int x;
  int y;
  unsigned char color;
  int xdir;
  int ydir;
  int patt;
  char prevdir;
} spriteAttr;

char buf[33];                   // work buffer
unsigned char bRunning = FALSE; // main game loop control
unsigned int anPos = 0;         // animation position
int anDir = 1;                  // 1, -1 counter direction
graphicsSetup g;                // graphics mode struct
spriteAttr sprAttr[32];         // sprite struct array
unsigned long score = 0;        // score field
unsigned int dotctr = 0;        // counter to decide when we're done with a screen
unsigned int energizerctr = 0;  // count down counter when an energizer is eaten
unsigned int ghostCtr = 0;      // score counter that adds up with each ghost being eaten


#define getRand256() ((unsigned char)(rand() % 256))


/* *******************************************************************************************************************************
   | Pull byte from VDP RAM                                                                                                      |
   *******************************************************************************************************************************
*/
unsigned char getVDPRAM(unsigned int addr) {
#ifdef GCC_COMPILED
    return screen_buffer[addr];
#else
    static unsigned int addr_2;
    static unsigned int addr_1;
    
    addr_2 = (addr >> 8);
    addr_1 = addr - (addr_2 << 8);

    PORTX81 = (unsigned char)addr_1;
    PORTX81 = (unsigned char)addr_2;
    return PORTX80;
#endif
}

/* ******************************************************************************************************************************
   | Retrieve character at position                                                                                             |
   ******************************************************************************************************************************
*/
unsigned char getCharAt(unsigned char x, unsigned char y) {
    static unsigned int addr;
    addr = g.NameTableAddr;

    if(g.graphicsMode == TEXTMODE)
      addr = addr + (y*40) + x;
    else
      addr = addr + (y<<5) + x;

    return getVDPRAM(addr);
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
void setBGColor() {
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
Uint32 getFGColorForChar(unsigned char ch) {
    unsigned char raw = getVDPRAM(g.ColorTableAddr + (ch >> 3));

    raw = raw >> 4; // fg color
    return getARGBFromTMS9118Color(raw);
}

/* ******************************************************************************************************************************
   | get background ARGB color for a given character                                                                            |
   ******************************************************************************************************************************
*/
Uint32 getBGColorForChar(unsigned char ch) {
    unsigned char raw = getVDPRAM(g.ColorTableAddr + (ch >> 3));

    raw = raw & 0x0f; // bg color
    return getARGBFromTMS9118Color(raw);
}

/* *****************************************************************************************************************************
   | plot character on SDL window                                                                                              |
   *****************************************************************************************************************************
*/
void plotCharOnSDLPixels(int xTMS9118, int yTMS9118, unsigned char c) {
  unsigned int pattern_addr = (c<<3) + g.PatternTableAddr;
  Uint32 fgCol = getFGColorForChar(c);
  Uint32 bgCol = getBGColorForChar(c);

  for(int i=0;i<8;i++) {  
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
void drawCharacters() {
  static int x;
  static int y;

  for(x=0;x<32;x++) {
    for(y=0;y<24;y++) {
      plotCharOnSDLPixels(x,y,getCharAt(x,y));
    }
  }
}

/* ******************************************************************************************************************************
   | draw sprites onto SDL window                                                                                               |
   ******************************************************************************************************************************
*/
void drawSprites() {
  static int x;
  static int y;
  static Uint32 addr;

  for(int i=32;i>=0;i--) {
      if(sprAttr[i].patt != -1 && sprAttr[i].color != TRANSPARENT) {
        addr = g.SpritePatternTableAddr;

        if(g.sprites16x16Enabled == TRUE) 
            addr = addr + (sprAttr[i].patt << 5);
        else
            addr = addr + (sprAttr[i].patt << 3);

          Uint32 clr = getARGBFromTMS9118Color(sprAttr[i].color);
          x = sprAttr[i].x;
          y = sprAttr[i].y;
          for(int j=0;j<16;j++) {
            unsigned char c = screen_buffer[addr+j];

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
          for(int j=16;j<32;j++) {
            unsigned char c = screen_buffer[addr+j];

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
   | draw out entire screen on SDL window                                                                                       |
   ******************************************************************************************************************************
*/
void updateEmulatedVDPScreen() {
  setBGColor();
  drawCharacters();
  drawSprites();
}

/* ******************************************************************************************************************************
   | draw out entire screen on SDL window                                                                                       |
   ******************************************************************************************************************************
*/
void SDLSetup() {
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
}


/* ******************************************************************************************************************************
   | Shutdown SDL                                                                                                               |
   ******************************************************************************************************************************
*/
void SDLShutdown() {
   SDL_DestroyTexture(texture);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}
#endif


/* ******************************************************************************************************************************
   | VDP register set (low level register change)                                                                               |
   ******************************************************************************************************************************
*/
void setVDPRegister(unsigned char reg,unsigned char dat) {
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
void setVDPRAM(unsigned int addr, unsigned char dat) {
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

    PORTX81 = (unsigned char)addr_1;
    PORTX81 = (unsigned char)addr_2;
    PORTX80 = dat;
#endif
}

/* ******************************************************************************************************************************
   | Set sound byte to left or right sound chip                                                                                 |
   ******************************************************************************************************************************
*/
void soundOut(unsigned char LeftOrRight, unsigned char dat) {
#ifdef GCC_COMPILED
#else
    if(LeftOrRight == LEFT_POS) 
        PORTX82 = dat;
    else
        PORTX83 = dat; 
#endif
}

/* ******************************************************************************************************************************
   | get left or right joystick position                                                                                        |
   ******************************************************************************************************************************
*/
unsigned char getJoystick(unsigned char LeftOrRight) {
#ifdef GCC_COMPILED 
    char ret=0;
    SDL_PumpEvents();
    const Uint8 *keystates  = SDL_GetKeyboardState( NULL );
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
    if(keystates[SDL_SCANCODE_ESCAPE])
      bRunning = FALSE;

    return 0;
#else
    if(LeftOrRight == LEFT_POS)
        return PORTX83;
    else
        return PORTX82;
#endif
}

/* ******************************************************************************************************************************
   | Put character on screen at position                                                                                        |
   ******************************************************************************************************************************
*/
void setCharacterAt(unsigned char x, unsigned char y, unsigned char c) {
    static unsigned int addr;
    addr = g.NameTableAddr;
    addr = addr + (y<<5) + x;

    setVDPRAM(addr,c);
}


/* ******************************************************************************************************************************
   | Write a string at screen position                                                                                          |
   ******************************************************************************************************************************
*/
void setCharactersAt(unsigned char x, unsigned char y, char* s) {
    static unsigned int addr;
    static unsigned char c;

    addr = g.NameTableAddr;
    addr = addr + (y<<5) + x;

    for(int i=0;i<strlen(s);i++) {
        c = *(s+i);
        setVDPRAM(addr+i, c);
    }
}

/* ******************************************************************************************************************************
   | load pattern table with binary data                                                                                        |
   ******************************************************************************************************************************
*/
void setCharPatternByArray(unsigned char pos, char* p, int l) {
  static unsigned int i;
  static unsigned int addr;

  addr = (pos<<3) + g.PatternTableAddr;

  for(i=0;i<l;i++)
    setVDPRAM(addr+i, *(p+i));
}

/* ******************************************************************************************************************************
   | set foreground and background of a group of characters                                                                     |
   ******************************************************************************************************************************
*/
unsigned char setCharacterGroupColor(unsigned char colorGroup, unsigned char foreground, unsigned char background) {
    static unsigned char c;

    if(colorGroup > 31 || foreground > WHITE || background > WHITE)
        return 0; 

    c = ((foreground << 4) & 0xf0) + (background & 0x0f);
    setVDPRAM(g.ColorTableAddr + colorGroup, c);
}

/* ******************************************************************************************************************************
   | load character patterns                                                                                                    |
   ******************************************************************************************************************************
*/
void setPatterns() {
   static unsigned int i;
   
   static char p0[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char a[8] =  {0x81,0x81,0x81,0x81,0x81,0x81,0x81,0x81};
   static char b[8] =  {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char c[8] =  {0x3c,0x42,0x81,0x81,0x81,0x81,0x81,0x81};
   static char d[8] =  {0x3F,0x40,0x80,0x80,0x80,0x80,0x40,0x3F};
   static char e[8] =  {0x81,0x81,0x81,0x81,0x81,0x81,0x42,0x3C};
   static char f[8] =  {0xFC,0x02,0x01,0x01,0x01,0x01,0x02,0xFC};
   static char g[8] =  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char h[8] =  {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80};
   static char ii[8] = {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char j[8] =  {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01};
   static char k[8] =  {0x3F,0x40,0x80,0x80,0x80,0x80,0x80,0x80};
   static char l[8] =  {0xFC,0x02,0x01,0x01,0x01,0x01,0x01,0x01};
   static char m[8] =  {0x01,0x01,0x01,0x01,0x01,0x01,0x02,0xFC};
   static char n[8] =  {0x80,0x80,0x80,0x80,0x80,0x80,0x40,0x3F};
   
   static char r[8] =  {0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00};
   static char q[8] =  {0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char s[8] =  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF};
   static char t[8] =  {0x38,0x7C,0xFE,0xFE,0xFE,0x7C,0x38,0x00};

   static char AU[8] = {0x78,0x84,0x84,0xFC,0x84,0x84,0x84,0x00};
   static char BU[8] = {0xF8,0x84,0x84,0xF8,0x84,0x84,0xF8,0x00};
   static char CU[8] = {0x78,0x84,0x80,0x80,0x80,0x84,0x78,0x00};
   static char DU[8] = {0xF8,0x84,0x84,0x84,0x84,0x84,0xF8,0x00};
   static char EU[8] = {0xFC,0x80,0x80,0xF8,0x80,0x80,0xFC,0x00};
   static char FU[8] = {0xFC,0x80,0x80,0xF8,0x80,0x80,0x80,0x00};
   static char GU[8] = {0x78,0x80,0x80,0xBC,0x84,0x84,0x78,0x00};
   static char HU[8] = {0x84,0x84,0x84,0xFC,0x84,0x84,0x84,0x00};
   static char IU[8] = {0xFC,0x30,0x30,0x30,0x30,0x30,0xFC,0x00};
   static char JU[8] = {0x7C,0x10,0x10,0x10,0x90,0x90,0x70,0x00};
   static char KU[8] = {0x84,0x98,0xA0,0xC0,0xA0,0x98,0x84,0x00};
   static char LU[8] = {0x80,0x80,0x80,0x80,0x80,0x80,0xFC,0x00};
   static char MU[8] = {0x84,0xCC,0xB4,0xB4,0x84,0x84,0x84,0x00};
   static char NU[8] = {0x84,0xC4,0xA4,0xA4,0x94,0x8C,0x84,0x00};
   static char OU[8] = {0x78,0x84,0x84,0x84,0x84,0x84,0x78,0x00};
   static char PU[8] = {0xF8,0x84,0x84,0xF8,0x80,0x80,0x80,0x00};
   static char QU[8] = {0x78,0x84,0x84,0x84,0x94,0x8C,0x7C,0x00};
   static char RU[8] = {0xF8,0x84,0x88,0xF0,0x88,0x88,0x84,0x00};
   static char SU[8] = {0x7C,0x80,0x80,0x78,0x04,0x04,0xF8,0x00};
   static char TU[8] = {0xFC,0x30,0x30,0x30,0x30,0x30,0x30,0x00};
   static char UU[8] = {0x84,0x84,0x84,0x84,0x84,0x84,0x78,0x00};
   static char VU[8] = {0x84,0x84,0x84,0x48,0x48,0x48,0x30,0x00};
   static char WU[8] = {0x84,0x84,0x84,0xB4,0xB4,0xCC,0x84,0x00};
   static char XU[8] = {0x84,0x84,0x48,0x30,0x48,0x84,0x84,0x00};
   static char YU[8] = {0x84,0x48,0x48,0x30,0x30,0x30,0x30,0x00};
   static char ZU[8] = {0xFC,0x08,0x10,0x10,0x20,0x40,0xFC,0x00};
   static char N0[8] = {0x78,0x8C,0x94,0xB4,0xA4,0xC4,0x78,0x00};
   static char N1[8] = {0x30,0x70,0x30,0x30,0x30,0x30,0xFC,0x00};
   static char N2[8] = {0x78,0x84,0x84,0x18,0x60,0x80,0xFC,0x00};
   static char N3[8] = {0x78,0x84,0x04,0x38,0x04,0x84,0x78,0x00};
   static char N4[8] = {0xCC,0xCC,0xCC,0xFC,0x0C,0x0C,0x0C,0x00};
   static char N5[8] = {0xFC,0x80,0x80,0x78,0x04,0x84,0x78,0x00};
   static char N6[8] = {0x38,0x40,0x80,0xF8,0x84,0x84,0x78,0x00};
   static char N7[8] = {0xFC,0x0C,0x18,0x18,0x30,0x30,0x60,0x00};
   static char N8[8] = {0x78,0x84,0x84,0x78,0x84,0x84,0x78,0x00};
   static char N9[8] = {0x78,0x84,0x84,0x7C,0x04,0x08,0x70,0x00};
   static char NC[8] = {0x00,0x30,0x30,0x00,0x00,0x30,0x30,0x00};
   static char ND[8] = {0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00};
   
   for(i=0;i<256;i++)                   // set all characters to blank
          setCharPatternByArray(i,p0,8);
                                        //     SHAPE  
   setCharPatternByArray('a',a,8);      //      | |   
                                        //      | |   
   setCharPatternByArray('b',b,8);      //      ___  
                                        //      ---   
   setCharPatternByArray('c',c,8);      //      .--.
                                        //      |  |  
   setCharPatternByArray('d',d,8);      //       ___
                                        //      {___  
   setCharPatternByArray('e',e,8);      //      |  |
                                        //      '--'  
   setCharPatternByArray('f',f,8);      //       ___
                                        //       ___}                 
   setCharPatternByArray('g',g,8);      //       ___

   setCharPatternByArray('h',h,8);      //       |
                                        //       |
   setCharPatternByArray('i',ii,8);     //       --
                                        //
   setCharPatternByArray('j',j,8);      //         |
                                        //         |
   setCharPatternByArray('k',k,8);      //       ___
                                        //      |
   setCharPatternByArray('l',l,8);      //       ___
                                        //          |
   setCharPatternByArray('m',m,8);      //          |
                                        //       ___|
   setCharPatternByArray('n',n,8);      //       |
                                        //       |___
   setCharPatternByArray('r',r,8);      //          .
   setCharPatternByArray('q',q,8);      //          _
   setCharPatternByArray('s',s,8);      //          _
   setCharPatternByArray('t',t,8);      //       energizer

   setCharPatternByArray('A',AU,8);      
   setCharPatternByArray('B',BU,8);      
   setCharPatternByArray('C',CU,8);      
   setCharPatternByArray('D',DU,8);      
   setCharPatternByArray('E',EU,8);      
   setCharPatternByArray('F',FU,8);      
   setCharPatternByArray('G',GU,8);      
   setCharPatternByArray('H',HU,8);      
   setCharPatternByArray('I',IU,8);      
   setCharPatternByArray('J',JU,8);      
   setCharPatternByArray('K',KU,8);      
   setCharPatternByArray('L',LU,8);      
   setCharPatternByArray('M',MU,8);      
   setCharPatternByArray('N',NU,8);      
   setCharPatternByArray('O',OU,8);      
   setCharPatternByArray('P',PU,8);      
   setCharPatternByArray('Q',QU,8);      
   setCharPatternByArray('R',RU,8);      
   setCharPatternByArray('S',SU,8);      
   setCharPatternByArray('T',TU,8);      
   setCharPatternByArray('U',UU,8);      
   setCharPatternByArray('V',VU,8);      
   setCharPatternByArray('W',WU,8);      
   setCharPatternByArray('X',XU,8);      
   setCharPatternByArray('Y',YU,8);      
   setCharPatternByArray('Z',ZU,8);      
   setCharPatternByArray('0',N0,8); 
   setCharPatternByArray('1',N1,8); 
   setCharPatternByArray('2',N2,8); 
   setCharPatternByArray('3',N3,8); 
   setCharPatternByArray('4',N4,8); 
   setCharPatternByArray('5',N5,8); 
   setCharPatternByArray('6',N6,8); 
   setCharPatternByArray('7',N7,8); 
   setCharPatternByArray('8',N8,8); 
   setCharPatternByArray('9',N9,8); 
   setCharPatternByArray(':',NC,8); 
   setCharPatternByArray('-',ND,8); 
}

/* ******************************************************************************************************************************
   | set TMS9118 hardware registers and graphic structure                                                                       |
   ******************************************************************************************************************************
*/
void setGraphicsMode() {
  static unsigned char b;

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
  g.SpriteAttrTableAddr = 0x3800;

  setVDPRegister(6, 0x03);
  g.SpritePatternTableAddr = 0x1800;

  b = g.textForegroundColor << 4 | g.backgroundColor;
  setVDPRegister(7, b);
}

/* ******************************************************************************************************************************
   | Load sprite pattern into pattern table                                                                                     |
   ******************************************************************************************************************************
*/
void setSpritePatternByArray(unsigned char patternNumber, char* p, char l) {
  static unsigned int addr;
  static unsigned int i;

  addr = g.SpritePatternTableAddr;
  if(g.sprites16x16Enabled == TRUE) 
      addr = addr + (patternNumber << 5);
  else
      addr = addr + (patternNumber << 3);

  for(i=0;i<l;i++) {
    setVDPRAM(addr+i,*(p+i));
  }
}

/* ******************************************************************************************************************************
   | move sprite data out of sprite attribute struct and move it into registers                                                 |
   ******************************************************************************************************************************
*/
void syncSpriteAttributesToHardware(unsigned char spriteNum) {
  static unsigned int addr;
  static unsigned char vert;
  static int horiz;
  static unsigned char b5;
  static int x;
  static unsigned int y;
  static unsigned char color;
  static unsigned char patt;

  patt = sprAttr[spriteNum].patt;
  x = sprAttr[spriteNum].x;
  y = sprAttr[spriteNum].y;
  color = sprAttr[spriteNum].color;
  addr = g.SpriteAttrTableAddr + (spriteNum << 2);

  b5 = color;
  vert = y ;

  if(x<0) {
    b5 = b5 | 0x80;
    x = x + 32;
  }
  horiz = x ;

  setVDPRAM(addr, vert);
  setVDPRAM(addr+1, (unsigned char)horiz);
  setVDPRAM(addr+2, patt<<2);
  setVDPRAM(addr+3, b5);
}


/* ******************************************************************************************************************************
   | clear TRS-80 screen by scrolling it 16 times                                                                               |
   ******************************************************************************************************************************
*/
void clearTRSScreen() {
  for(int i=0;i<16;i++)
    printf("\n");
}


/* ******************************************************************************************************************************
   | draw dots into maze                                                                                                        |
   ******************************************************************************************************************************
*/
void drawDots() {
  static unsigned char k;

  for(k=2;k<31;k++) {
    setCharacterAt(k,3,'r');
    setCharacterAt(k,23,'q');
  }
  setCharacterAt(1,23,'s');

  for(k=3;k<23;k++) {
    setCharacterAt(2,k,'r');
    setCharacterAt(30,k,'r');
    setCharacterAt(5,k,'r');
    setCharacterAt(27,k,'r');
    if(k < 21) {
      setCharacterAt(8,k,'r');
      setCharacterAt(24,k,'r');
    }
  } 

  for(k=5;k<27;k++) 
    setCharacterAt(k,20,'r');

  for(k=9;k<24;k++) {
    setCharacterAt(k,17,'r');
    setCharacterAt(k,6,'r');
  }
  for(k=2;k<5;k++) {
    setCharacterAt(k,12,'r');
    setCharacterAt(k+25,12,'r');
  }
  for(k=17;k<21;k++) {
    setCharacterAt(16,k,'r');
    setCharacterAt(16,k-11,'r');
  }
  for(k=11;k<22;k++) {
    setCharacterAt(k,9,'r');
    setCharacterAt(k,14,'r');
  }
  for(k=9;k<14;k++) {
    setCharacterAt(11,k,'r');
    setCharacterAt(21,k,'r');
  }
  for(k=8;k<12;k++) {
    setCharacterAt(k,12,'r');
    setCharacterAt(k+13,12,'r');
  }
  for(k=15;k<=17;k++) 
    setCharacterAt(k,17,' ');

  for(k=15;k<17;k++) {
    setCharacterAt(13,k,'r');
    setCharacterAt(19,k,'r');
  }

  setCharacterAt(2,5,'t');   // energizer
  setCharacterAt(2,19,'t');  // energizer
  setCharacterAt(30,5,'t');  // energizer
  setCharacterAt(30,19,'t'); // energizer
}

/* ******************************************************************************************************************************
   | make all the characters white (except the ones in the alphabetic text)                                                     |
   ******************************************************************************************************************************
*/
void setEverythingWhite() {
  static unsigned char j;

  for(j=0;j<32;j++)
     setCharacterGroupColor(j, WHITE, BLACK);  // set all characters to blue on black
  for(j=8;j<=11;j++)
     setCharacterGroupColor(j, DARKRED, BLACK);   // set chars 64 - 95 to red on black 
}

/* ******************************************************************************************************************************
   | set the color groups at the right colors for the maze                                                                      |
   ******************************************************************************************************************************
*/
void setMazeColors() {
  static unsigned char j;

  for(j=0;j<32;j++)
     setCharacterGroupColor(j, DARKBLUE, BLACK);  // set all characters to blue on black
  for(j=8;j<=11;j++)
     setCharacterGroupColor(j, DARKRED, BLACK);   // set chars 64 - 95 to red on black 
  for(j=6;j<=7;j++)
     setCharacterGroupColor(j, WHITE, BLACK);     // set chars 48 - 63 to white on black

   setCharacterGroupColor(14,WHITE,BLACK);
}

/* ********************************************************************************************************************************
   | update the score and show it on the screen (sprintf is expensive, so we don't do it all the time -- just when score updates) |
   ********************************************************************************************************************************
*/
void updateScore(int incr) {
  score = score + incr;
  sprintf(buf, "%08ld", score);
  setCharactersAt(24,0,buf);
}

/* ***************************
   | draw characters of maze |
   ***************************
*/
void drawMaze() {
  static unsigned char j;
  static unsigned char k;

  setMazeColors();

  // clear screen
  for(k=0;k<24;k++)
      for(j=0;j<32;j++)
          setCharacterAt(j,k,' ');

   for(j=3;j<=9;j++) {
       setCharacterAt(0,j,'a');
       setCharacterAt(31,j,'a');
   }

   for(j=14;j<=22;j++) {
       setCharacterAt(0,j,'a');
       setCharacterAt(31,j,'a');    
   }

   for(j=0;j<32;j++) {
       setCharacterAt(j,1,'g');
   }

   setCharactersAt(18,0,"SCORE:");
   updateScore(0);

   for(j=5;j<=9;j++) {
       setCharacterAt(3, j, 'a');
       setCharacterAt(28, j, 'a');
   }

   for(j=14;j<=20;j++) {
       setCharacterAt(3, j, 'a');
       setCharacterAt(28, j, 'a');
   }

   for(j=5;j<=17;j++) {
       setCharacterAt(6, j, 'a');
       setCharacterAt(25, j, 'a');
   }

   for(j=8;j<=9;j++) {
       setCharacterAt(9, j, 'a');
       setCharacterAt(22,j, 'a');
   }

   setCharacterAt(9,14,'a');
   setCharacterAt(22,14,'a');

   for(j=10;j<22;j++) 
       setCharacterAt(j,4,'b');
   
   for(j=10;j<=13;j++) {
       setCharacterAt(j,7,'b');
       setCharacterAt(j+8,7,'b');
   }

   setCharacterAt(13, 10, 'i');
   setCharacterAt(14, 10, 'i');
   setCharacterAt(17, 10, 'i');
   setCharacterAt(18, 10, 'i');
   setCharacterAt(12, 11, 'h');
   setCharacterAt(19, 11, 'j');

   for(j=13;j<=18;j++) 
      setCharacterAt(j,12, 'g');

   setCharacterAt(10,15, 'b');
   setCharacterAt(21,15, 'b');
   setCharacterAt(15,15, 'b');
   setCharacterAt(16,15, 'b');

   for(j=10;j<=13;j++) {
    setCharacterAt(j,18, 'b');
    setCharacterAt(j+8,18, 'b');
   }

   for(j=7;j<=24;j++) 
    setCharacterAt(j,21,'b');

  setCharacterAt(0,2,'c');
  setCharacterAt(3,4,'c');
  setCharacterAt(6,4,'c');
  setCharacterAt(25,4,'c');
  setCharacterAt(28,4,'c');
  setCharacterAt(31,2,'c');
  setCharacterAt(0,13,'c');
  setCharacterAt(3,13,'c');
  setCharacterAt(9,13,'c');
  setCharacterAt(22,13,'c');
  setCharacterAt(28,13,'c');
  setCharacterAt(31,13,'c');

  setCharacterAt(0,10,'e');
  setCharacterAt(3,10,'e');
  setCharacterAt(9,10,'e');
  setCharacterAt(22,10,'e');
  setCharacterAt(28,10,'e');
  setCharacterAt(31,10,'e');
  setCharacterAt(0,23,'e');
  setCharacterAt(3,21,'e');
  setCharacterAt(6,18,'e');
  setCharacterAt(25,18,'e');
  setCharacterAt(31,23,'e');
  setCharacterAt(28,21,'e');

  setCharacterAt(9, 4, 'd');
  setCharacterAt(9, 18, 'd');
  setCharacterAt(6, 21, 'd');
  setCharacterAt(17,18, 'd');
  setCharacterAt(20,15, 'd');
  setCharacterAt(14,15, 'd');
  setCharacterAt(17,7,'d');

  setCharacterAt(22,4,'f');
  setCharacterAt(17,15,'f');
  setCharacterAt(14,18,'f');
  setCharacterAt(22,18,'f');
  setCharacterAt(25,21,'f');
  setCharacterAt(11,15,'f');
  setCharacterAt(14,7,'f');

  setCharacterAt(12,10,'k');
  setCharacterAt(9,7,'k');
  setCharacterAt(19,10,'l');
  setCharacterAt(22,7,'l');
  setCharacterAt(12,12,'n');
  setCharacterAt(9,15,'n');
  setCharacterAt(19,12,'m');
  setCharacterAt(22,15,'m');

  setCharacterAt(9,15,'n');
  setCharacterAt(22,15,'m');
  
  drawDots();  
}

/* ******************************************************************************************************************************
   | can this sprite go south?                                                                                                  |
   ******************************************************************************************************************************
*/
unsigned char canGoSouth(unsigned char spriteNum) {
    static int x;
    static int y;

    x = sprAttr[spriteNum].x;
    y = sprAttr[spriteNum].y;

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
unsigned char canGoNorth(unsigned char spriteNum) {
    static int x;
    static int y;

    x = sprAttr[spriteNum].x;
    y = sprAttr[spriteNum].y;

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
unsigned char canGoEast(unsigned char spriteNum) {
    static int x;
    static int y;

    x = sprAttr[spriteNum].x;
    y = sprAttr[spriteNum].y;

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
unsigned char canGoWest(unsigned char spriteNum) {
    static int x;
    static int y;

    x = sprAttr[spriteNum].x;
    y = sprAttr[spriteNum].y;

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
   | do nothing X times                                                                                                         |
   ******************************************************************************************************************************
*/
void hold(unsigned int x) {
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

/* ******************************************************************************************************************************
   | set ghost direction - NORTH                                                                                                |
   ******************************************************************************************************************************
*/
void goNorth(int i) {
        sprAttr[i].ydir = -2;
        sprAttr[i].xdir = 0;
        sprAttr[i].prevdir = NORTH;
        sprAttr[i-4].patt = PATT_NORMAL_GHOST_EYES_N;
}

/* ******************************************************************************************************************************
   | set ghost direction - SOUTH                                                                                                |
   ******************************************************************************************************************************
*/
void goSouth(int i) {
        sprAttr[i].ydir = 2;
        sprAttr[i].xdir = 0;
        sprAttr[i].prevdir = SOUTH;
        sprAttr[i-4].patt = PATT_NORMAL_GHOST_EYES_S;
}

/* ******************************************************************************************************************************
   | set ghost direction - EAST                                                                                                 |
   ******************************************************************************************************************************
*/
void goEast(int i) {
        sprAttr[i].xdir = 2;
        sprAttr[i].ydir = 0;
        sprAttr[i].prevdir = EAST;
        sprAttr[i-4].patt = PATT_NORMAL_GHOST_EYES_E;
}

/* ******************************************************************************************************************************
   | set ghost direction - WEST                                                                                                 |
   ******************************************************************************************************************************
*/
void goWest(int i) {
        sprAttr[i].xdir = -2;
        sprAttr[i].ydir = 0;
        sprAttr[i].prevdir = WEST;
        sprAttr[i-4].patt = PATT_NORMAL_GHOST_EYES_W;
}


/* ******************************************************************************************************************************
   | randomly pick an available ghost direction                                                                                 |
   ******************************************************************************************************************************
*/
void setAvailableGhostDirection(int i) {
  static unsigned char bFoundOne;
  static unsigned char r;

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
void ghostsNormal() {
    static int i;
    sprAttr[RED_GHOST_SPRITENUM].color = DARKRED;
    sprAttr[BROWN_GHOST_SPRITENUM].color = DARKYELLOW;
    sprAttr[CYAN_GHOST_SPRITENUM].color = CYAN;
    sprAttr[PINK_GHOST_SPRITENUM].color = LIGHTRED;
    for(i=RED_GHOST_SPRITENUM;i<=BROWN_GHOST_SPRITENUM;i++) {
      sprAttr[i-4].color = WHITE;
    }
}

/* ******************************************************************************************************************************
   | Move ghosts one step                                                                                                       |
   ******************************************************************************************************************************
*/
void moveGhosts() {
  static int ctr = 0;
  static int i;

  for(i=RED_GHOST_SPRITENUM;i<=BROWN_GHOST_SPRITENUM;i++) {
      if(sprAttr[i].prevdir == NORTH && canGoNorth(i) == FALSE) 
        setAvailableGhostDirection(i);
      else
        if(sprAttr[i].prevdir == SOUTH && canGoSouth(i) == FALSE) 
          setAvailableGhostDirection(i);
        else
          if(sprAttr[i].prevdir == EAST && canGoEast(i) == FALSE) 
            setAvailableGhostDirection(i);
          else 
            if(sprAttr[i].prevdir == WEST && canGoWest(i) == FALSE) 
              setAvailableGhostDirection(i);

      // randomly override current ghost direction every once in a while
      if(getRand256() > 240) {
        setAvailableGhostDirection(i);
      }

      // while the ghosts are in the box, make it more likely for them to go north
      if(sprAttr[i].y == 84 && sprAttr[i].x >= 114 && sprAttr[i].x <= 138) {
          if(canGoNorth(i)) {
            sprAttr[i].ydir = -2;
            sprAttr[i].xdir = 0;
            sprAttr[i-4].patt = PATT_NORMAL_GHOST_EYES_N;
          }
      }

      if(getRand256() > 64 - (i<<3) )  { // slows down ghosts randomly
          sprAttr[i].x = sprAttr[i].x + sprAttr[i].xdir;
          sprAttr[i].y = sprAttr[i].y + sprAttr[i].ydir;
      }

      if(energizerctr == 0) {
        ctr++;
        if( ctr < 24 )  
          sprAttr[i].patt = PATT_NORMAL_GHOST_1;
        else 
          sprAttr[i].patt = PATT_NORMAL_GHOST_2;
        
        if(ctr > 47) ctr = 0;
      }
      else {
        sprAttr[i].patt = PATT_SCARED_GHOST;
      }
  }

  for(i=RED_GHOST_SPRITENUM;i<=BROWN_GHOST_SPRITENUM;i++) {
    sprAttr[i-4].x = sprAttr[i].x;
    sprAttr[i-4].y = sprAttr[i].y;
    if(energizerctr > 0) {
      sprAttr[i-4].color = TRANSPARENT;
      sprAttr[i].color = DARKBLUE;
    }
  }

  if(energizerctr == 1) {
    ghostsNormal();
    ghostCtr = 0;
  }
}


/* ******************************************************************************************************************************
   | Put ghosts back in their starting spot                                                                                     |
   ******************************************************************************************************************************
*/
void putGhostsInBox() {
   sprAttr[RED_GHOST_SPRITENUM].x = 120;
   sprAttr[RED_GHOST_SPRITENUM].y = 40;
   sprAttr[RED_GHOST_SPRITENUM].color = DARKRED;
   sprAttr[RED_GHOST_SPRITENUM].prevdir = EAST;
   sprAttr[RED_GHOST_SPRITENUM].xdir = 2;
   sprAttr[RED_GHOST_SPRITENUM].ydir = 0;
   sprAttr[RED_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_1;
   sprAttr[RED_GHOST_EYES_SPRITENUM].color = WHITE;
   sprAttr[RED_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;

   sprAttr[CYAN_GHOST_SPRITENUM].x = 130;
   sprAttr[CYAN_GHOST_SPRITENUM].y = 84;
   sprAttr[CYAN_GHOST_SPRITENUM].color = CYAN;
   sprAttr[CYAN_GHOST_SPRITENUM].prevdir = EAST;
   sprAttr[CYAN_GHOST_SPRITENUM].xdir = 2;
   sprAttr[CYAN_GHOST_SPRITENUM].ydir = 0;
   sprAttr[CYAN_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_1;
   sprAttr[CYAN_GHOST_EYES_SPRITENUM].color = WHITE;
   sprAttr[CYAN_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;

   sprAttr[PINK_GHOST_SPRITENUM].x = 122;
   sprAttr[PINK_GHOST_SPRITENUM].y = 84;
   sprAttr[PINK_GHOST_SPRITENUM].color = LIGHTRED;
   sprAttr[PINK_GHOST_SPRITENUM].prevdir = WEST;
   sprAttr[PINK_GHOST_SPRITENUM].xdir = -2;
   sprAttr[PINK_GHOST_SPRITENUM].ydir = 0;
   sprAttr[PINK_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_2;
   sprAttr[PINK_GHOST_EYES_SPRITENUM].color = WHITE;
   sprAttr[PINK_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;

   sprAttr[BROWN_GHOST_SPRITENUM].x = 114;
   sprAttr[BROWN_GHOST_SPRITENUM].y = 84;
   sprAttr[BROWN_GHOST_SPRITENUM].color = DARKYELLOW;
   sprAttr[BROWN_GHOST_SPRITENUM].prevdir = WEST;
   sprAttr[BROWN_GHOST_SPRITENUM].xdir = -2;
   sprAttr[BROWN_GHOST_SPRITENUM].ydir = 0;
   sprAttr[BROWN_GHOST_SPRITENUM].patt = PATT_NORMAL_GHOST_2;
   sprAttr[BROWN_GHOST_EYES_SPRITENUM].color = WHITE;
   sprAttr[BROWN_GHOST_EYES_SPRITENUM].patt = PATT_NORMAL_GHOST_EYES_N;
}


/* ******************************************************************************************************************************
   | playLeft / playRight - play a note on the TRS80MXS                                                                         |
   ******************************************************************************************************************************
*/
#ifdef GCC_COMPILED
    #define playLeft(NOTE)     //printf("playLeft(NOTE) is undefined for GCC_COMPILED mode\n");
    #define playRight(NOTE)    //printf("playRight(NOTE) is undefined for GCC_COMPILED mode\n");
#else
    #define playLeft(NOTE) PORTX82 = notes[NOTE].b1; PORTX82 = notes[NOTE].b2;
    #define playRight(NOTE) PORTX83 = notes[NOTE].b1; PORTX83 = notes[NOTE].b2;
#endif


/* ******************************************************************************************************************************
   | cycle through all audio ports on the TRS80MXS and shut off all 4 voices per chip                                           |
   ******************************************************************************************************************************
*/
void audioSilence() {
  static int j;

#ifndef GCC_COMPILED
    for(j=249;j<=255;j=j+2) {
      PORTX85 = j;
      PORTX84 = j;
      PORTX83 = j;
      PORTX82 = j;
    }
#endif
}


/* ******************************************************************************************************************************
   | set left and right volumes up                                                                                              |
   ******************************************************************************************************************************
*/
void volumeup() {
  soundOut(LEFT_POS,9);  soundOut(RIGHT_POS,9);
}


/* ******************************************************************************************************************************
   | turn off audio, wait a certain amount of time, reset the volume back up ("musical rest")                                   |
   ******************************************************************************************************************************
*/
void rest(unsigned int x) {
  audioSilence(); 
  hold(x); 
  volumeup();
}

#define EIGHTHNOTE 1000
#define SIXTEENTHNOTE 500
#define THIRTYSECONDNOTE 250
#define SIXTYFOURTHNOTE 125
#define ONETWENTYEIGHTHNOTE 63
#define TWOFIFTYSIXTHNOTE 32


/* ******************************************************************************************************************************
   | Make pacman wakka wakka sound                                                                                              |
   ******************************************************************************************************************************
*/
void wakka() {
  volumeup();
  playLeft(C2);
  hold(4);
  playLeft(A3);
  hold(4);
  playLeft(C2);
  hold(4);
  playLeft(C1);
  hold(4);
  playLeft(A0);
  hold(4);
  playLeft(C1);
  hold(4);
  playLeft(G1);
  hold(4);
  audioSilence();
}

/* ******************************************************************************************************************************
   | movePacman one step and eat dot if one is under him                                                                        |
   ******************************************************************************************************************************
*/
void movePacman() {
    static unsigned char c;
    static int x;
    static int y;
    static int xd;
    static int yd;
    static char cc;
    static unsigned char xx;
    static unsigned char yy;

    x = sprAttr[PACMAN_SPRITENUM].x;
    y = sprAttr[PACMAN_SPRITENUM].y;
    xd = sprAttr[PACMAN_SPRITENUM].xdir;
    yd = sprAttr[PACMAN_SPRITENUM].ydir;    

    if((xd < 0 && canGoWest(PACMAN_SPRITENUM)) || (xd > 0 && canGoEast(PACMAN_SPRITENUM)))
        sprAttr[PACMAN_SPRITENUM].x = x + xd;
    if((yd < 0 && canGoNorth(PACMAN_SPRITENUM)) || (yd > 0 && canGoSouth(PACMAN_SPRITENUM)))
        sprAttr[PACMAN_SPRITENUM].y = y + yd;

    if(x < -8)
        sprAttr[PACMAN_SPRITENUM].x = -8;
    if(x > 248)
        sprAttr[PACMAN_SPRITENUM].x = 248;

    if(y < 16)
        sprAttr[PACMAN_SPRITENUM].y = 16;
    if(y > 174)
        sprAttr[PACMAN_SPRITENUM].y = 174;

      if(x == 0)
           sprAttr[PACMAN_SPRITENUM].x = 238;
      else
          if(x == 240)
            sprAttr[PACMAN_SPRITENUM].x = 2;
   
    anPos = anPos + anDir;
    if(anPos > 1 || anPos < 1) 
      anDir = -anDir;

    if(yd < 0) 
        sprAttr[PACMAN_SPRITENUM].patt = anPos + NORTH_PACMAN_PAT_OFFSET;
    else 
        if(yd > 0) 
            sprAttr[PACMAN_SPRITENUM].patt = anPos + SOUTH_PACMAN_PAT_OFFSET;
        else
            if(xd < 0) 
                sprAttr[PACMAN_SPRITENUM].patt = anPos + WEST_PACMAN_PAT_OFFSET;
            else 
                sprAttr[PACMAN_SPRITENUM].patt = anPos + EAST_PACMAN_PAT_OFFSET;

    x = x >> 3;
    y = y >> 3;

    xx = (unsigned char) x + 1;
    yy = (unsigned char) y + 1;

    cc = getCharAt(xx,yy);
    if(cc == 'r' || cc == 't') {
      updateScore(10);
      dotctr++;
      wakka();
      setCharacterAt(xx,yy,' ');
      if(cc == 't') 
        energizerctr = 200;
    }

    if(energizerctr > 0)
      energizerctr--;

    if(y == 21) {
      yy++;
      cc = getCharAt(xx,yy);
      if(cc == 'q') {
        updateScore(10);
        dotctr++;
        wakka();
        setCharacterAt(xx,yy,'s');
      }
    }
}


/* ******************************************************************************************************************************
   | read joystick or keyboard and set pacman sprite direction                                                                  |
   ******************************************************************************************************************************
*/
void checkControls() {
        static unsigned char k;
        k = getJoystick(LEFT_POS);

        if(k == J_E && canGoEast(PACMAN_SPRITENUM)) {
          sprAttr[PACMAN_SPRITENUM].xdir=2;
          sprAttr[PACMAN_SPRITENUM].ydir=0;
        }
        else
        if(k == J_N && canGoNorth(PACMAN_SPRITENUM)) {
          sprAttr[PACMAN_SPRITENUM].ydir=-2;
          sprAttr[PACMAN_SPRITENUM].xdir=0;
        }
        else
        if(k == J_W && canGoWest(PACMAN_SPRITENUM)) {
          sprAttr[PACMAN_SPRITENUM].xdir=-2;
          sprAttr[PACMAN_SPRITENUM].ydir=0;
        }
        else
        if(k == J_S && canGoSouth(PACMAN_SPRITENUM)) {
          sprAttr[PACMAN_SPRITENUM].ydir=2;
          sprAttr[PACMAN_SPRITENUM].xdir=0;
        }
        else
        if(k == J_BUTTON)
              bRunning = FALSE;
}

/* ******************************************************************************************************************************
   | play introduction music                                                                                                    |
   ******************************************************************************************************************************
*/
void introMusic() {
  #ifdef GCC_COMPILED
      return;
  #endif

  volumeup();

  playLeft(C0);                    playRight( C2);                  hold(EIGHTHNOTE);
                                   playRight( C3);                  hold(EIGHTHNOTE);
  playLeft(C1);                    playRight( G2);                  hold(EIGHTHNOTE);
                                   playRight( E2);                  hold(EIGHTHNOTE);
  playLeft(C0);                    playRight( C3);                  hold(EIGHTHNOTE);
                                   playRight( G2);                  hold(EIGHTHNOTE);
  playLeft(C1);                    playRight( E2);                  hold(EIGHTHNOTE);
                                                                    hold(EIGHTHNOTE);

  playLeft(CS0);                   playRight( CS2);                 hold(EIGHTHNOTE);
                                   playRight( CS3);                 hold(EIGHTHNOTE);
  playLeft(CS1);                   playRight( GS2);                 hold(EIGHTHNOTE);
                                   playRight( F2);                  hold(EIGHTHNOTE);
  playLeft(CS0);                   playRight( CS3);                 hold(EIGHTHNOTE);
                                   playRight( GS2);                 hold(EIGHTHNOTE);
  playLeft(CS1);                   playRight( F2);                  hold(EIGHTHNOTE);
                                                                    hold(EIGHTHNOTE);

  playLeft(C0);                    playRight( C2);                  hold(EIGHTHNOTE);
                                   playRight( C3);                  hold(EIGHTHNOTE);
  playLeft(C1);                    playRight( G2);                  hold(EIGHTHNOTE);
                                   playRight( E2);                  hold(EIGHTHNOTE);
  playLeft(C0);                    playRight( C3);                  hold(EIGHTHNOTE);
                                   playRight( G2);                  hold(EIGHTHNOTE);
  playLeft(C1);                    playRight( E2);                  hold(EIGHTHNOTE);
                                                                    hold(EIGHTHNOTE);

  playLeft(G0);                    playRight( F2);                  hold(EIGHTHNOTE);
                                   playRight( FS2);                 hold(EIGHTHNOTE);
                                   playRight( G2);                  hold(SIXTEENTHNOTE);
                                                                    rest(TWOFIFTYSIXTHNOTE); 
  playLeft(A1);                    playRight( G2);                  hold(EIGHTHNOTE);
                                   playRight( GS2);                 hold(EIGHTHNOTE);
                                   playRight( A3);                  hold(SIXTEENTHNOTE);
                                                                    rest(TWOFIFTYSIXTHNOTE);
  playLeft(B1);                    playRight( A3);                  hold(EIGHTHNOTE);
                                   playRight( AS3);                 hold(EIGHTHNOTE);
                                   playRight( B3);                  hold(SIXTEENTHNOTE);
                                                                    rest(TWOFIFTYSIXTHNOTE);
  playLeft(C1);                    playRight( C3);                  hold(EIGHTHNOTE);
                                                                    hold(EIGHTHNOTE);
                                                                    hold(EIGHTHNOTE);
                                                                    hold(EIGHTHNOTE);

  audioSilence();
}

/* ******************************************************************************************************************************
   | resets the maze back to normal                                                                                             |
   ******************************************************************************************************************************
*/
void resetMap() {
      energizerctr = 0;
      putGhostsInBox();
      ghostsNormal();  
      sprAttr[PACMAN_SPRITENUM].x = 120;
      sprAttr[PACMAN_SPRITENUM].y = 128;
}


/* ******************************************************************************************************************************
   | decide if all 260 dots have been eaten, if so, flash the maze                                                              |
   ******************************************************************************************************************************
*/
void checkForAllDotsGone() {
      static char cc;
      if(dotctr == 260) {
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
      dotctr = 0;
      drawDots();
      resetMap();
    }
}

/* ******************************************************************************************************************************
   | dead pac-man animation, and map reset                                                                                      |
   ******************************************************************************************************************************
*/
void pacmanDead() {
  static int i;

  sprAttr[PACMAN_SPRITENUM].xdir = 0;
  sprAttr[PACMAN_SPRITENUM].ydir = 0;

    for(i=PATT_PACMAN_DYING_START;i<=PATT_PACMAN_DYING_END;i++) {
      hold(500);
      sprAttr[PACMAN_SPRITENUM].patt = i;
      syncSpriteAttributesToHardware(PACMAN_SPRITENUM);
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
   | check collision between pacman and ghosts                                                                                  |
   ******************************************************************************************************************************
*/
void checkCollisions()  {
  static int px;
  static int py;
  static int pxl;
  static int pyl;
  static int i;
  static int gx;
  static int gy;
  static int gxl;
  static int gyl;

  px = sprAttr[PACMAN_SPRITENUM].x+2;
  py = sprAttr[PACMAN_SPRITENUM].y+2;
  pxl = px + 14;
  pyl = py + 14;

  sprAttr[PACMAN_SPRITENUM].color = DARKYELLOW;

  for(i=RED_GHOST_SPRITENUM;i<=BROWN_GHOST_SPRITENUM;i++) {
    gx = sprAttr[i].x+2;
    gy = sprAttr[i].y+2;
    gxl = sprAttr[i].x+14;
    gyl = sprAttr[i].y+14;
    if (px  < gxl &&
        pxl > gx &&
        py  < gyl &&
        pyl > gy) {
      if(energizerctr == 0)
          pacmanDead();
      else {
        if(i == RED_GHOST_SPRITENUM) 
          sprAttr[i].color = DARKRED;
        else if(i == BROWN_GHOST_SPRITENUM)
          sprAttr[i].color = DARKYELLOW;
        else if(i == CYAN_GHOST_SPRITENUM)
          sprAttr[i].color = CYAN;
        else
          sprAttr[i].color = LIGHTRED;

          sprAttr[i].xdir = 0;
          sprAttr[i].ydir = -2;
          sprAttr[i].x = 120;
          sprAttr[i].y = 84;
          sprAttr[i].prevdir = WEST;
          ghostCtr = ghostCtr + 400;
          updateScore(ghostCtr);
      }    
    }      
  }
}

/* ******************************************************************************************************************************
   | clear maze and turn sprites off                                                                                            |
   ******************************************************************************************************************************
*/
void clearMazeShutOffSprites() {
  static unsigned char k;
  static unsigned char j;

   for(k=0;k<24;k++)
     for(j=0;j<32;j++)
        setCharacterAt(j,k,' ');

   for(j=0;j<=MAX_SPRITENUM;j++) {
    sprAttr[j].color = BLACK;
    syncSpriteAttributesToHardware(j);
   }
}


/* ******************************************************************************************************************************
   | main code                                                                                                                  |
   ******************************************************************************************************************************
*/
int main()
{
   int j;
   unsigned int k;
   unsigned int i;

   #ifdef GCC_COMPILED
   SDLSetup();
   #endif

   clearTRSScreen();

   printf(".---------------------------------------.\n");
   printf("(                PACMAN!!               )\n");
   printf("(           YYYY-MM-DD-HH-MM-SS         )\n");
   printf("(              CKSUM               )\n");
   printf("'---------------------------------------'\n\n");
   printf("THIS PROGRAM REQUIRES THE TRS-80 GRAPHICS ");
   printf("AND SOUND CARD V2.0+.\n\n");

   printf("1...");
   audioSilence();

   printf("2...");
   initNoteData();

   printf("3...");
   srand(getRand256());

   printf("4...");

   g.graphicsMode = GRAPHICSMODE1;
   g.externalVideoEnabled = FALSE;
   g.interruptsEnabled = FALSE;
   g.spritesMagnified = FALSE;
   g.sprites16x16Enabled = TRUE;
   g.textForegroundColor = WHITE;
   g.backgroundColor = BLACK;
   setGraphicsMode();

   printf("5...");

   memset(sprAttr,0x0,sizeof(sprAttr));
   for(i=0;i<32;i++)
    sprAttr[i].patt = -1;

   sprAttr[PACMAN_SPRITENUM].x = 120;
   sprAttr[PACMAN_SPRITENUM].y = 128;
   sprAttr[PACMAN_SPRITENUM].patt = anPos;
   
   clearMazeShutOffSprites();
   putGhostsInBox();

   printf("6...");
   setPatterns();

   printf("7...");
   drawMaze();

   printf("8..."); 

   static char pacman_e1[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_e2[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x78,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xE0,0x00,0x00,0x00,0xE0,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_e3[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7E,0x7C,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0x00};
   static char pacman_n1[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_n2[32] = {0x00,0x00,0x00,0x18,0x38,0x3C,0x7C,0x7C,0x7E,0x7E,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0x00,0x30,0x38,0x78,0x7C,0x7C,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_n3[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x70,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x1C,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_w1[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_w2[32] = {0x00,0x00,0x03,0x0F,0x1F,0x1F,0x07,0x00,0x00,0x00,0x07,0x1F,0x1F,0x0F,0x03,0x00,0x00,0x00,0xE0,0xF8,0xFC,0xFC,0xFE,0xFE,0x1E,0xFE,0xFE,0xFC,0xFC,0xF8,0xE0,0x00};
   static char pacman_w3[32] = {0x00,0x00,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x03,0x00,0x00,0x00,0xE0,0xF8,0xFC,0xFC,0x7E,0x3E,0x1E,0x3E,0x7E,0xFC,0xFC,0xF8,0xE0,0x00};
   static char pacman_s1[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7F,0x7F,0x7F,0x7F,0x7F,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0xFC,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_s2[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7E,0x7E,0x7E,0x7C,0x7C,0x3C,0x38,0x18,0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0xFC,0xFC,0x7C,0x7C,0x78,0x38,0x30,0x00,0x00};
   static char pacman_s3[32] = {0x00,0x00,0x07,0x1F,0x3F,0x3F,0x7E,0x7C,0x78,0x70,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xF8,0xF8,0xFC,0x7C,0x3C,0x1C,0x0C,0x00,0x00,0x00,0x00,0x00};

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

   printf("9..."); 

   static char normal_ghost_1[32] =      {0x00,0x03,0x0F,0x1F,0x33,0x21,0x21,0x21,0x73,0x7F,0x7F,0x7F,0x7F,0x7F,0x6C,0x44,0x00,0x80,0xE0,0xF0,0x98,0x08,0x08,0x08,0x9C,0xFC,0xFC,0xFC,0xFC,0xFC,0xEC,0xC4};
   static char normal_ghost_2[32] =      {0x00,0x03,0x0F,0x1F,0x33,0x21,0x21,0x21,0x73,0x7F,0x7F,0x7F,0x7F,0x7F,0x7B,0x31,0x00,0x80,0xE0,0xF0,0x98,0x08,0x08,0x08,0x9C,0xFC,0xFC,0xFC,0xFC,0xFC,0xDC,0x88};
   static char normal_ghost_eyes_n[32] = {0x00,0x00,0x00,0x00,0x00,0x12,0x1E,0x1E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0xF0,0xF0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char normal_ghost_eyes_s[32] = {0x00,0x00,0x00,0x00,0x0C,0x1E,0x1E,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xF0,0xF0,0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char normal_ghost_eyes_e[32] = {0x00,0x00,0x00,0x00,0x0C,0x1E,0x18,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xF0,0xC0,0xC0,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char normal_ghost_eyes_w[32] = {0x00,0x00,0x00,0x00,0x0C,0x1E,0x06,0x06,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0xF0,0x30,0x30,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char scared_ghost[32] =        {0x00,0x03,0x0F,0x1F,0x3F,0x3F,0x3F,0x79,0x79,0x7F,0x7F,0x66,0x59,0x7F,0x6E,0x46,0x00,0xC0,0xF0,0xF8,0xFC,0xFC,0xFC,0x9E,0x9E,0xFE,0xFE,0x66,0x9A,0xFE,0x76,0x62};

   setSpritePatternByArray(PATT_NORMAL_GHOST_1, normal_ghost_1,32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_2, normal_ghost_2,32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_N, normal_ghost_eyes_n, 32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_S, normal_ghost_eyes_s, 32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_E, normal_ghost_eyes_e, 32);
   setSpritePatternByArray(PATT_NORMAL_GHOST_EYES_W, normal_ghost_eyes_w, 32);
   setSpritePatternByArray(PATT_SCARED_GHOST, scared_ghost, 32);

   printf("10...");

   static char pacman_dying_start[32] = {0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x70,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x1C,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_dying_1[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x78,0x7C,0x7E,0x3F,0x3F,0x1F,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x3C,0x7C,0xFC,0xF8,0xF8,0xF0,0xC0,0x00};
   static char pacman_dying_2[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x7C,0x7F,0x3F,0x1F,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x7C,0xFC,0xF8,0xF0,0xC0,0x00};
   static char pacman_dying_3[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x70,0x7F,0x3F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0xFC,0xF8,0xE0,0x00};
   static char pacman_dying_4[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x7F,0x3F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0xFC,0xF8,0xE0};
   static char pacman_dying_5[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x0F,0x3F,0x7F,0x3E,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF8,0xFC,0xF8,0x60};
   static char pacman_dying_6[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x0F,0x1F,0x7F,0x7E,0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xE0,0xF0,0xFC,0xFC,0x78};
   static char pacman_dying_7[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x07,0x07,0x0F,0x1F,0x0E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0xC0,0xE0,0xF0,0xE0};
   static char pacman_dying_8[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x03,0x03,0x03,0x07,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0xC0,0x80};
   static char pacman_dying_9[32]     = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
   static char pacman_dying_end[32]   = {0x00,0x00,0x00,0x04,0x02,0x10,0x08,0x00,0x00,0x30,0x00,0x00,0x08,0x10,0x02,0x04,0x00,0x00,0x00,0x20,0x40,0x08,0x10,0x00,0x00,0x0C,0x00,0x00,0x10,0x08,0x40,0x20};

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
   
   sprAttr[PACMAN_SPRITENUM].color = DARKYELLOW;

   printf("11...");
   introMusic(); 

   printf("12...\n");  

   bRunning = TRUE;

   while(bRunning == TRUE) {
        for(j=0; j <= MAX_SPRITENUM ;j++)
          syncSpriteAttributesToHardware(j);

        checkControls();
        movePacman();
        checkForAllDotsGone();
        moveGhosts();
        checkCollisions();

        #ifdef GCC_COMPILED
            updateEmulatedVDPScreen();
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
        #endif

        #ifdef GCC_COMPILED
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
            hold(300);
        #endif
   }

   clearMazeShutOffSprites();

   setCharactersAt(12,11,"GAME OVER");

   printf("DONE!");

   #ifdef GCC_COMPILED
   SDLShutdown();
   #endif

   exit(-1);
}
