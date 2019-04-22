/*
  #define L1_RED()    {digitalWriteFast(5,HIGH);digitalWriteFast(7,LOW); digitalWriteFast(6,LOW);}
  #define L1_GREEN()  {digitalWriteFast(5,LOW); digitalWriteFast(7,LOW); digitalWriteFast(6,HIGH);}
  #define L1_BLUE()   {digitalWriteFast(5,LOW); digitalWriteFast(7,HIGH);digitalWriteFast(6,LOW);}
  #define L1_YELLOW() {digitalWriteFast(5,HIGH);digitalWriteFast(7,LOW); digitalWriteFast(6,HIGH);}
  #define L1_CYAN()   {digitalWriteFast(5,LOW); digitalWriteFast(7,HIGH);digitalWriteFast(6,HIGH);}
  #define L1_VIOLET() {digitalWriteFast(5,HIGH);digitalWriteFast(7,HIGH);digitalWriteFast(6,LOW);}
  #define L1_WHITE()  {digitalWriteFast(5,HIGH);digitalWriteFast(7,HIGH);digitalWriteFast(6,HIGH);}
  #define L1_BLACK()  {digitalWriteFast(5,LOW); digitalWriteFast(7,LOW); digitalWriteFast(6,LOW);}
*/
#define L2_RED()    {digitalWriteFast(2,HIGH);digitalWriteFast(4,LOW); digitalWriteFast(3,LOW);}
#define L2_BLUE()   {digitalWriteFast(2,LOW); digitalWriteFast(4,HIGH);digitalWriteFast(3,LOW);}
#define L2_GREEN()  {digitalWriteFast(2,LOW); digitalWriteFast(4,LOW); digitalWriteFast(3,HIGH);}
#define L2_CYAN()   {digitalWriteFast(2,LOW); digitalWriteFast(4,HIGH);digitalWriteFast(3,HIGH);}
#define L2_VIOLET() {digitalWriteFast(2,HIGH);digitalWriteFast(4,HIGH);digitalWriteFast(3,LOW);}
#define L2_YELLOW() {digitalWriteFast(2,HIGH);digitalWriteFast(4,LOW); digitalWriteFast(3,HIGH);}
#define L2_WHITE()  {digitalWriteFast(2,HIGH);digitalWriteFast(4,HIGH);digitalWriteFast(3,HIGH);}
#define L2_BLACK()  {digitalWriteFast(2,LOW); digitalWriteFast(4,LOW); digitalWriteFast(3,LOW);}


//                        0 - disconnected
//                        1 - disconnected
#define LED2_GREEN        2 /* PORTD BIT 0 */
#define LED2_RED          3 /* PORTA BIT 12 */
#define LED2_BLUE         4 /* PORTA BIT 13 */
//                        5 - disconnected - PORTD BIT 7
//                        6 - disconnected - PORTD BIT 4
//                        7 - disconnected - PORTD BIT 2
#define INTERUPT_TO_TRS80 8 /* PORTD BIT 3 */
#define D3                9 /* PORTC BIT 3 */
#define D4               10 /* PORTC BIT 4 */
#define D6               11 /* PORTC BIT 6 */
#define D7               12 /* PORTC BIT 7 */
#define D5               13 /* PORTC BIT 5 */
//                       14 - disconnected
#define D0               15 /* PORTC BIT 0 */
//                       16 - disconnected
//                       17 - disconnected
//                       18 - disconnected
//                       19 - disconnected
//                       20 - disconnected
//                       21 - disconnected
#define D1               22 /* PORTC BIT 1 */
#define D2               23 /* PORTC BIT 2 */
//                       24 - disconnected
#define FF_CLR           25 /* PORTA BIT 5 */
#define _37ECRD          26 /* PORTA BIT 14 */
#define _37E0WR          27 /* PORTA BIT 15 */
#define _37E4WR          28 /* PORTA BIT 16 */
#define FF_PRE           29 /* PORTB BIT 18 */
#define _37E8RD          30 /* PORTB BIT 19 */
#define _37E0RD          31 /* PORTB BIT 10 */
#define _37E4RD          32 /* PORTB BIT 11 */
#define _37E8WR          33 /* PORTE BIT 24 */
#define _37ECWR          34 /* PORTE BIT 25 */
#define _A0              35 /* PORTC BIT 8 */
#define _A1              36 /* PORTC BIT 9 */
#define SYSRES           37 /* PORTC BIT 10 */
#define NOTHING1         38 /* PORTC BIT 11 */
#define NOTHING2         39 /* PORTA BIT 17 */

#define IN 0
#define OUT 1

typedef struct  {
  volatile int trackNum = 0;
  volatile int sectorNum = 0;
  volatile int dataRegister = 0;
  volatile int commandRegister;
  volatile int statusRegister;
  volatile int byteCtr = 0;
  volatile int afterSectorBusyCtr = 0;
  volatile int busyCtr = 0;
  volatile int DRQCtr = 0;

  volatile int iIndexHole = 1;
  volatile int iTrackDirection = IN;
  volatile int motorRunningCtr;

  volatile int sectorsRead = 0;
  volatile int statusRegisterChecks = 0;
  volatile int currentCommand;
  File diskFile;
  String sDiskFileName;
}
drivesettings;


/*
  stuff for video screen
  #define OLED_MOSI   20
  #define OLED_CLK   21
  #define OLED_DC    18
  #define OLED_CS    17
  #define OLED_RESET 19
  Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
*/
