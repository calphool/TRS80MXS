#define P34 34
#define WIFI_OUT 34

#define P35 35
#define WIFI_IN 35

#define P32 32
#define D7 32

#define P33 33
#define D6 33

#define P25 25
#define D5 25

#define P26 26
#define D4 26

#define P27 27
#define D3 27

#define P14 14
#define D2 14

#define P12 12
#define D1 15

#define P13 13
#define D0 13

#define P0 0
#define RGBLED1_G 0

#define P4 4
#define RGBLED1_B 4

#define P16 16
#define P17 17

#define P5 5
#define RGBLED2_R 5

#define P18 18
#define RGBLED2_G 18

#define P19 19
#define RGBLED2_B 19

#define P21 21
#define RGBLED1_R 21

#define P22 22
#define IC6A_CLR 22

#define P23 23
#define IC6A_PRE 23

#define IN 0
#define OUT 1


#define L1_RED()    {digitalWrite(21,HIGH);digitalWrite(4,LOW); digitalWrite(0,LOW);}
#define L1_BLUE()   {digitalWrite(21,LOW); digitalWrite(4,HIGH);digitalWrite(0,LOW);}
#define L1_GREEN()  {digitalWrite(21,LOW); digitalWrite(4,LOW); digitalWrite(0,HIGH);}
#define L1_CYAN()   {digitalWrite(21,LOW); digitalWrite(4,HIGH);digitalWrite(0,HIGH);}
#define L1_VIOLET() {digitalWrite(21,HIGH);digitalWrite(4,HIGH);digitalWrite(0,LOW);}
#define L1_YELLOW() {digitalWrite(21,HIGH);digitalWrite(4,LOW); digitalWrite(0,HIGH);}
#define L1_WHITE()  {digitalWrite(21,HIGH);digitalWrite(4,HIGH);digitalWrite(0,HIGH);}
#define L1_BLACK()  {digitalWrite(21,LOW); digitalWrite(4,LOW); digitalWrite(0,LOW);}

#define L2_RED()    {digitalWrite(5,HIGH);digitalWrite(19,LOW); digitalWrite(18,LOW);}
#define L2_BLUE()   {digitalWrite(5,LOW); digitalWrite(19,HIGH);digitalWrite(18,LOW);}
#define L2_GREEN()  {digitalWrite(5,LOW); digitalWrite(19,LOW); digitalWrite(18,HIGH);}
#define L2_CYAN()   {digitalWrite(5,LOW); digitalWrite(19,HIGH);digitalWrite(18,HIGH);}
#define L2_VIOLET() {digitalWrite(5,HIGH);digitalWrite(19,HIGH);digitalWrite(18,LOW);}
#define L2_YELLOW() {digitalWrite(5,HIGH);digitalWrite(19,LOW); digitalWrite(18,HIGH);}
#define L2_WHITE()  {digitalWrite(5,HIGH);digitalWrite(19,HIGH);digitalWrite(18,HIGH);}
#define L2_BLACK()  {digitalWrite(5,LOW); digitalWrite(19,LOW); digitalWrite(18,LOW);}
