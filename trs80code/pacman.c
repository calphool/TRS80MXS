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
    unsigned char getCharAt(unsigned char x, unsigned char y);
    unsigned char getVDPRAM(unsigned int addr);
    #define SCREEN_WIDTH 512
    #define SCREEN_HEIGHT 384
#endif


// port addresses
#define LEFT_POS 0x82
#define RIGHT_POS 0x83

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


// pattern setting function macro
#define SETPATTERN(XXX,YYY) strcpy(buf,YYY); setCharPattern(XXX,buf);


// sprite identifiers
#define PACMAN_SPRITENUM 0
#define RED_GHOST_SPRITENUM 1
#define CYAN_GHOST_SPRITENUM 2
#define PINK_GHOST_SPRITENUM 3
#define BROWN_GHOST_SPRITENUM 4

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


typedef struct {
  int x;
  int y;
  unsigned char color;
  int xdir;
  int ydir;
  int patt;
} spriteAttr;


unsigned char bRunning = FALSE;  // main game loop control
unsigned int anPos = 0;         // animation position
int anDir = 1;                  // 1, -1 counter direction
graphicsSetup g;                // graphics mode struct
spriteAttr sprAttr[32];         // sprite struct array
unsigned int score = 0;
unsigned int dotctr = 0; 


/* *************************************************************
   | get the more-or-less random value of the R register       |
   *************************************************************
*/
unsigned char getRRegister() {
#ifdef GCC_COMPILED
    return rand() % 256;
#else
    #pragma asm
    ld a,r
    ld h,0
    ld l,a
    #pragma endasm
#endif
}



#ifdef GCC_COMPILED

void *memset32(void *m, uint32_t val, size_t count) {
    uint32_t *buf = m;
    while(count--) *buf++ = val;
    return m;
}


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

void setBGColor() {
      memset32(pixels, getARGBFromTMS9118Color(VDPRegisters[7] & 0x0f), SCREEN_WIDTH*SCREEN_HEIGHT);
}



void plotSDLPixel(int x, int y, Uint32 color) {    // x = 0 - 255, y = 0 - 191
    memset32(pixels + (x<<1) + y*SCREEN_WIDTH*2, color, 2); 
    memset32(pixels + (x<<1) + y*SCREEN_WIDTH*2 + SCREEN_WIDTH, color, 2);     
}

Uint32 getFGColorForChar(unsigned char ch) {
    unsigned char raw = getVDPRAM(g.ColorTableAddr + (ch >> 3));

    raw = raw >> 4; // fg color
    return getARGBFromTMS9118Color(raw);
}

Uint32 getBGColorForChar(unsigned char ch) {
    unsigned char raw = getVDPRAM(g.ColorTableAddr + (ch >> 3));

    raw = raw & 0x0f; // bg color
    return getARGBFromTMS9118Color(raw);
}


void plotCharOnSDLPixels(int xTMS9118, int yTMS9118, unsigned char c) {
  unsigned int pattern_addr = (c<<3) + g.PatternTableAddr;
  Uint32 fgCol = getFGColorForChar(c);
  Uint32 bgCol = getBGColorForChar(c);

  for(int i=0;i<8;i++) {  
    c = screen_buffer[pattern_addr+i]; // now c = something like 0x55
    if(c >= 128) {
      plotSDLPixel(xTMS9118*8, yTMS9118*8+i,fgCol);
      c=c-128;
    }
    else 
      plotSDLPixel(xTMS9118*8, yTMS9118*8+i,bgCol);

    if(c >= 64) {
      plotSDLPixel(xTMS9118*8+1, yTMS9118*8+i,fgCol);
      c=c-64;
    }
   else 
      plotSDLPixel(xTMS9118*8+1, yTMS9118*8+i,bgCol);

    if(c >= 32) {
      plotSDLPixel(xTMS9118*8+2, yTMS9118*8+i,fgCol);
      c=c-32;
    }
   else 
      plotSDLPixel(xTMS9118*8+2, yTMS9118*8+i,bgCol);

    if(c >= 16) {
      plotSDLPixel(xTMS9118*8+3, yTMS9118*8+i,fgCol);
      c=c-16;
    }
   else 
      plotSDLPixel(xTMS9118*8+3, yTMS9118*8+i,bgCol);

    if(c >= 8) {
      plotSDLPixel(xTMS9118*8+4, yTMS9118*8+i,fgCol);
      c=c-8;
    }
   else 
      plotSDLPixel(xTMS9118*8+4, yTMS9118*8+i,bgCol);

    if(c >= 4) {
      plotSDLPixel(xTMS9118*8+5, yTMS9118*8+i,fgCol);
      c=c-4;
    }
   else 
      plotSDLPixel(xTMS9118*8+5, yTMS9118*8+i,bgCol);

    if(c >= 2) {
      plotSDLPixel(xTMS9118*8+6, yTMS9118*8+i,fgCol);
      c=c-2;
    }
   else 
      plotSDLPixel(xTMS9118*8+6, yTMS9118*8+i,bgCol);

    if(c >= 1) {
      plotSDLPixel(xTMS9118*8+7, yTMS9118*8+i,fgCol);
      c=c-1;
    }
   else 
      plotSDLPixel(xTMS9118*8+7, yTMS9118*8+i,bgCol);

  }
}


void drawCharacters() {
  for(int x=0;x<32;x++) {
    for(int y=0;y<24;y++) {
      plotCharOnSDLPixels(x,y,getCharAt(x,y));
    }
  }
}


void drawSprites() {
  static int x;
  static int y;

  Uint32 addr = g.SpritePatternTableAddr;

  for(int i=0;i<32;i++) {
      if(sprAttr[i].patt != -1) {
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

void updateEmulatedVDPScreen() {
  if(!bRunning)
    return;

  setBGColor();
  drawCharacters();
  drawSprites();
}



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

void SDLShutdown() {
   SDL_DestroyTexture(texture);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
}


#endif


/* *************************************************************
   | VDP register set (low level register change)              |
   *************************************************************
*/
void setVDPRegister(unsigned char reg,unsigned char dat) {
#ifdef GCC_COMPILED
    VDPRegisters[reg] = dat;
    updateEmulatedVDPScreen();
#else
    PORTX81 = dat;
    reg=reg+128;
    PORTX81 = reg;
#endif
}


/* *************************************************************
   | Set byte in VDP RAM                                       |
   *************************************************************
*/
void setVDPRAM(unsigned int addr, unsigned char dat) {
#ifdef GCC_COMPILED
    screen_buffer[addr] = dat;
    updateEmulatedVDPScreen();
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

/* *************************************************************
   | Pull byte from VDP RAM                                    |
   *************************************************************
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

/* *************************************************************
   | Set sound byte to left or right sound chip                |
   *************************************************************
*/
void soundOut(unsigned char LeftOrRight, unsigned char dat) {
#ifdef GCC_COMPILED
    #warning soundOut() is undefined for GCC_COMPILED mode
    //printf("soundOut() is undefined for GCC_COMPILED mode\n");
#else
    if(LeftOrRight == LEFT_POS) 
        PORTX82 = dat;
    else
        PORTX83 = dat; 
#endif
}

/* *************************************************************
   | get left or right joystick position                       |
   *************************************************************
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

/* *************************************************************
   | Put character on screen at position                       |
   *************************************************************
*/
void setCharacterAt(unsigned char x, unsigned char y, unsigned char c) {
    static unsigned int addr;
    addr = g.NameTableAddr;

    if(g.graphicsMode == TEXTMODE)
      addr = addr + (y*40) + x;
    else
      addr = addr + (y<<5) + x;

    setVDPRAM(addr,c);
}


/* *************************************************************
   | Retrieve character at position                            |
   *************************************************************
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

/* *************************************************************
   | Write a string at screen position                         |
   *************************************************************
*/
void setCharactersAt(unsigned char x, unsigned char y, char* s) {
    static unsigned int addr;
    static unsigned char c;

    addr = g.NameTableAddr;
    if(g.graphicsMode == TEXTMODE)
      addr = addr + (y*40) + x;
    else
      addr = addr + (y<<5) + x;

    for(int i=0;i<strlen(s);i++) {
        c = *(s+i);
        setVDPRAM(addr+i, c);
    }
}


void setScreenColor(unsigned char textForegroundColor, unsigned char backgroundColor) {
    setVDPRegister(7,textForegroundColor << 4 | backgroundColor);
}


void setCharPattern(unsigned char pos, char* patt) {
    static char duple[3];
    static unsigned char d;
    static unsigned char e;
    static unsigned char f;
    static unsigned int addr;

    addr = (pos<<3) + g.PatternTableAddr;
    duple[2]=0x0;
    d = 0;
    e = strlen(patt);
    for(f=0;f<e;f=f+2) {
        duple[0] = *(patt+f);
        duple[1] = *(patt+f+1);
        setVDPRAM(addr+d,(unsigned char) strtol(duple, NULL, 16));
        d++;
    }
}


unsigned char setCharacterGroupColor(unsigned char colorGroup, unsigned char foreground, unsigned char background) {
    static unsigned char c;

    if(colorGroup > 31 || foreground > WHITE || background > WHITE)
        return 0; 

    c = ((foreground << 4) & 0xf0) + (background & 0x0f);
    setVDPRAM(g.ColorTableAddr + colorGroup, c);
}


void setPatterns() {
   unsigned int i;
   unsigned char buf[17];
   
   for(i=0;i<256;i++)
       setCharPattern(i,"0000000000000000");   

                                        //     SHAPE  
   SETPATTERN('a',"8181818181818181");  //      | |   
                                        //      | |   

   SETPATTERN('b',"FF000000000000FF");  //      ___  
                                        //      ---   

   SETPATTERN('c',"3C42818181818181");  //      .--.
                                        //      |  |  

   SETPATTERN('d',"3F4080808080403F");  //       ___
                                        //      {___  

   SETPATTERN('e',"818181818181423C");  //      |  |
                                        //      '--'  

   SETPATTERN('f',"FC020101010102FC");  //       ___
                                        //       ___}
                                               
   SETPATTERN('g',"00000000000000FF");  //       ___


   SETPATTERN('h',"8080808080808080");  //       |
                                        //       |

   SETPATTERN('i',"FF00000000000000");  //       --
                                        //

   SETPATTERN('j',"0101010101010101");  //         |
                                        //         |

   SETPATTERN('k',"3F40808080808080");  //       ___
                                        //      |

   SETPATTERN('l',"FC02010101010101");  //       ___
                                        //          |

   SETPATTERN('m',"01010101010102FC");  //          |
                                        //       ___|

   SETPATTERN('n',"808080808080403F");  //       |
                                        //       |___

   SETPATTERN('r',"C0C0000000000000");  //          .
   SETPATTERN('q',"C0C00000000000FF");  //          _
   SETPATTERN('s',"00000000000000FF");  //          _

   SETPATTERN('A',"788484FC84848400");
   SETPATTERN('B',"F88484F88484F800");
   SETPATTERN('C',"7884808080847800");
   SETPATTERN('D',"F88484848484F800");
   SETPATTERN('E',"FC8080F88080FC00");
   SETPATTERN('F',"FC8080F880808000");
   SETPATTERN('G',"788080BC84847800");
   SETPATTERN('H',"848484FC84848400");
   SETPATTERN('I',"FC3030303030FC00");
   SETPATTERN('J',"7C10101090907000");
   SETPATTERN('K',"8498A0C0A0988400");
   SETPATTERN('L',"808080808080FC00");
   SETPATTERN('M',"84CCB4B484848400");
   SETPATTERN('N',"84C4A4A4948C8400");
   SETPATTERN('O',"7884848484847800");
   SETPATTERN('P',"F88484F880808000");
   SETPATTERN('Q',"78848484948C7C00");
   SETPATTERN('R',"F88488F088888400");
   SETPATTERN('S',"7C8080780404F800");
   SETPATTERN('T',"FC30303030303000");
   SETPATTERN('U',"8484848484847800");
   SETPATTERN('V',"8484844848483000");
   SETPATTERN('W',"848484B4B4CC8400");
   SETPATTERN('X',"8484483048848400");
   SETPATTERN('Y',"8448483030303000");
   SETPATTERN('Z',"FC0810102040FC00");
   SETPATTERN('0',"788C94B4A4C47800");
   SETPATTERN('1',"307030303030FC00");
   SETPATTERN('2',"788484186080FC00");
   SETPATTERN('3',"7884043804847800");
   SETPATTERN('4',"CCCCCCFC0C0C0C00");
   SETPATTERN('5',"FC80807804847800");
   SETPATTERN('6',"384080F884847800");
   SETPATTERN('7',"FC0C181830306000");
   SETPATTERN('8',"7884847884847800");
   SETPATTERN('9',"7884847C04087000");
   SETPATTERN(':',"0030300000303000");
   SETPATTERN('-',"0000007E00000000");
}


void setGraphicsMode() {
  unsigned char b;

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


void setSpritePattern(unsigned char spriteNumber, char* patt) {
  static char duple[3];
  static unsigned char d;
  static unsigned char x;
  static unsigned int addr;
  static unsigned char slen;
  duple[2] = 0x0;


  addr = g.SpritePatternTableAddr;
  if(g.sprites16x16Enabled == TRUE) 
      addr = addr + (spriteNumber << 5);
  else
      addr = addr + (spriteNumber << 3);
  
  d=0;
  slen = strlen(patt);
  for(x=0;x<slen;x=x+2) {
        duple[0] = *(patt+x);
        duple[1] = *(patt+x+1);
        setVDPRAM(addr+d,(unsigned char)strtol(duple, NULL, 16));
        d++;
  }
}


 void setSpriteAttribute(unsigned char spriteNum, unsigned char patternNumber) {
  static unsigned int addr;
  static unsigned char vert;
  static int horiz;
  static unsigned char b5;
  static int x;
  static unsigned int y;
  static unsigned char color;


  sprAttr[spriteNum].patt = patternNumber;
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
  setVDPRAM(addr+2, patternNumber<<2);
  setVDPRAM(addr+3, b5);
}



void clearTRSScreen() {
  for(int i=0;i<16;i++)
    printf("\n");
}


void drawDots() {
  static unsigned char k;

  // dots


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

}


void setEverythingWhite() {
  static unsigned char j;
  static unsigned char k;
  for(j=0;j<32;j++)
     setCharacterGroupColor(j, WHITE, BLACK);  // set all characters to blue on black
  for(j=8;j<=11;j++)
     setCharacterGroupColor(j, DARKRED, BLACK);   // set chars 64 - 95 to red on black 
}

void setMazeColors() {
  static unsigned char j;
  static unsigned char k;
  for(j=0;j<32;j++)
     setCharacterGroupColor(j, DARKBLUE, BLACK);  // set all characters to blue on black
  for(j=8;j<=11;j++)
     setCharacterGroupColor(j, DARKRED, BLACK);   // set chars 64 - 95 to red on black 
  for(j=6;j<=7;j++)
     setCharacterGroupColor(j, WHITE, BLACK);     // set chars 48 - 63 to white on black

   setCharacterGroupColor(14,WHITE,BLACK);
}

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
       //setCharacterAt(j,23,'g');
   }

   setCharactersAt(18,0,"SCORE:");

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

    if(y >= 174)
      return FALSE;

    return FALSE;
}


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


    if(y <= 16)
      return FALSE;

    return FALSE;
}


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



void movePacman() {
    static unsigned char c;
    static int x;
    static int y;
    static int xd;
    static int yd;
    static char cc;

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
        setSpriteAttribute(PACMAN_SPRITENUM, anPos + NORTH_PACMAN_PAT_OFFSET);
    else 
        if(yd > 0)
            setSpriteAttribute(PACMAN_SPRITENUM, anPos + SOUTH_PACMAN_PAT_OFFSET);
        else
            if(xd < 0)
                setSpriteAttribute(PACMAN_SPRITENUM, anPos + WEST_PACMAN_PAT_OFFSET);
            else
                setSpriteAttribute(PACMAN_SPRITENUM, anPos + EAST_PACMAN_PAT_OFFSET);

    x = x >> 3;
    y = y >> 3;
    cc = getCharAt(x+1,y+1);
    if(cc == 'r') {
      score = score + 10;
      dotctr++;
    }

    setCharacterAt(x+1,y+1,' ');
    if(y == 21) {
      cc = getCharAt(x+1,y+2);
      if(cc == 'q') {
        score = score + 10;
        dotctr++;
      }
      setCharacterAt(x+1,y+2,'s');
    }

    if(dotctr == 260) {
      for(cc = 0; cc < 3; cc++) {
        setEverythingWhite();
        #ifdef GCC_COMPILED
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        #endif
        hold(1000);
        setMazeColors();
        #ifdef GCC_COMPILED
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        #endif
        hold(1000);
      }
      sprAttr[PACMAN_SPRITENUM].x = 120;
      sprAttr[PACMAN_SPRITENUM].y = 128;
      dotctr = 0;
      drawDots();
    }
}


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



void audioSilence() {
  soundOut(LEFT_POS,255); soundOut(RIGHT_POS,255);
  soundOut(LEFT_POS,253); soundOut(RIGHT_POS,253);
  soundOut(LEFT_POS,251); soundOut(RIGHT_POS,251);
  soundOut(LEFT_POS,249); soundOut(RIGHT_POS,249);
}


void volumeup() {
  soundOut(LEFT_POS,9);  soundOut(RIGHT_POS,9);
}


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


#ifdef GCC_COMPILED
    #define playLeft(NOTE)     printf("playLeft(NOTE) is undefined for GCC_COMPILED mode\n");
    #define playRight(NOTE)    printf("playRight(NOTE) is undefined for GCC_COMPILED mode\n");
#else
    #define playLeft(NOTE) PORTX82 = notes[NOTE].b1; PORTX82 = notes[NOTE].b2;
    #define playRight(NOTE) PORTX83 = notes[NOTE].b1; PORTX83 = notes[NOTE].b2;
#endif

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


int main()
{
   int j;
   unsigned int k;
   unsigned int i;

   char buf[33];


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
   srand(getRRegister());

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
   sprAttr[PACMAN_SPRITENUM].x = 120;
   sprAttr[PACMAN_SPRITENUM].y = 128;
   sprAttr[PACMAN_SPRITENUM].color = BLACK;
   setSpriteAttribute(PACMAN_SPRITENUM,anPos);

   for(i=0;i<32;i++)
    sprAttr[i].patt = -1;


   printf("6...");
   setPatterns();

   printf("7...");
   drawMaze();

   printf("8..."); 

   setSpritePattern(0, "071F3F7F7FFFFFFFFFFFFF7F7F3F1F07E0F8FCDEFEFFFF80FFFFFFFEFEFCF8E0"); // PACMAN, mouth closed (east)
   setSpritePattern(1, "071F3F7F7FFFFFFFFFFFFF7F7F3F1F07E0F8FCDEFFFCE080E0FCFFFEFEFCF8E0"); // PACMAN, mouth open 1 (east)
   setSpritePattern(2, "071F3F7F7FFFFFFFFFFFFF7F7F3F1F07E0F8FCDEF8E080000080E0F8FEFCF8E0"); // PACMAN, mouth open 2 (east)
   
   setSpritePattern(3, "061E3E7E7EEEFEFFFFFFFF7F7F3F1F07E0F8FCFEFEFFFFFFFFFFFFFEFEFCF8E0"); // PACMAN, mouth closed (north)
   setSpritePattern(4, "08183C7C7CEEFEFFFFFFFF7F7F3F1F0720387C7E7EFFFFFFFFFFFFFEFEFCF8E0"); // PACMAN, mouth open 1 (north)
   setSpritePattern(5, "0010307878ECFCFEFFFFFF7F7F3F1F0700080C1E1E3F3F7FFFFFFFFEFEFCF8E0"); // PACMAN, mouth open 2 (north)
   
   setSpritePattern(6, "071F3F7B7FFFFF01FFFFFF7F7F3F1F07E0F8FCFEFEFFFFFFFFFFFFFEFEFCF8E0"); // PACMAN, mouth closed (west)
   setSpritePattern(7, "071F3F7BFF3F0701073FFF7F7F3F1F07E0F8FCFEFEFFFFFFFFFFFFFEFEFCF8E0"); // PACMAN, mouth open 1 (west)
   setSpritePattern(8, "071F3F7B1F0701000001071F7F3F1F07E0F8FCFEFEFFFFFFFFFFFFFEFEFCF8E0"); // PACMAN, mouth open 2 (west)
   
   setSpritePattern(9, "071F3F7F7FFFFFFFFFFEEE7E7E3E1E06E0F8FCFEFEFFFFFFFFFFFFFEFEFCF8E0"); // PACMAN, mouth closed (south)
   setSpritePattern(10,"071F3F7F7FFFFFFFFFFEEE7C7C3C1808E0F8FCFEFEFFFFFFFFFFFF7E7E7C3820"); // PACMAN, mouth open 1 (south)
   setSpritePattern(11,"071F3F7F7FFFFFFFFEFCEC7878301000E0F8FCFEFEFFFFFF7F3F3F1E1E0C0800"); // PACMAN, mouth open 2 (south)
   
   setSpritePattern(12,"070F0F1D193A383F3F3F3F3F3F3F1D08F0F8F8DCCCAE8EFEFEFEFEFEFEFEDC88"); // GHOST (1)
   setSpritePattern(13,"0F1F1F3F3375717F7F7F7F7F7F7F6E24E0F0F0F8985C1CFCFCFCFCFCFCFCEC48"); // GHOST (2)
   
   sprAttr[PACMAN_SPRITENUM].color = DARKYELLOW;

   printf("9...");
   introMusic(); 

   printf("10...\n");  

   bRunning = TRUE;

   while(bRunning == TRUE) {
        #ifdef GCC_COMPILED
            SDL_UpdateTexture(texture, NULL, pixels, SCREEN_WIDTH * sizeof(Uint32));
        #endif

        checkControls();
        movePacman();


        sprintf(buf, "%08d", score);
        setCharactersAt(24,0,buf);

        #ifdef GCC_COMPILED
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        #endif
   }

   printf("DONE!");

   #ifdef GCC_COMPILED
   SDLShutdown();
   #endif

   exit(-1);
}
