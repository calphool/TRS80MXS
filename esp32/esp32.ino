#include "defines.h"

char dataDirection = 2;

void dataIn() {
  if(dataDirection != IN) {
    //Serial.printf("  D7-D0 = INPUT\n");  Serial.flush();
    pinMode(D7, INPUT);
    pinMode(D6, INPUT);
    pinMode(D5, INPUT);
    pinMode(D4, INPUT);
    pinMode(D3, INPUT);
    pinMode(D2, INPUT);
    pinMode(D1, INPUT);
    pinMode(D0, INPUT);
    dataDirection = IN;
  }
}

void dataOut() {
  if(dataDirection != OUT) {
    //Serial.printf("  D7-D0 = OUTPUT\n");  Serial.flush();
    pinMode(D7, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D0, OUTPUT);
    dataDirection = OUT;
  }
}

void setPinDirection() {
  Serial.printf("  WIFI_OUT = INPUT\n");  Serial.flush();
  pinMode(WIFI_OUT, INPUT);
  Serial.printf("  WIFI_IN  = INPUT\n");  Serial.flush();
  pinMode(WIFI_IN, INPUT);
  
  dataIn();
  Serial.printf("  RGBLED1_G= OUTPUT\n");  Serial.flush();
  pinMode(RGBLED1_G, OUTPUT);
  Serial.printf("  RGBLED1_B= OUTPUT\n");  Serial.flush();
  pinMode(RGBLED1_B, OUTPUT);
  Serial.printf("  RGBLED1_R= OUTPUT\n");  Serial.flush();
  pinMode(RGBLED1_R, OUTPUT);
  Serial.printf("  RGBLED2_G= OUTPUT\n");  Serial.flush();
  pinMode(RGBLED2_G, OUTPUT);
  Serial.printf("  RGBLED2_B= OUTPUT\n");  Serial.flush();
  pinMode(RGBLED2_B, OUTPUT);
  Serial.printf("  RGBLED2_R= OUTPUT\n");  Serial.flush();
  pinMode(RGBLED2_R, OUTPUT);
  Serial.printf("  IC6A_CLR = OUTPUT\n");  Serial.flush();
  pinMode(IC6A_CLR, OUTPUT);
  Serial.printf("  IC6A_PRE = OUTPUT\n");  Serial.flush();
  pinMode(IC6A_PRE, OUTPUT);
}

unsigned char getByte() {
  unsigned char c = 0;
  if(digitalRead(D7)==HIGH) c+=128;
  if(digitalRead(D6)==HIGH) c+=64;
  if(digitalRead(D5)==HIGH) c+=32;
  if(digitalRead(D4)==HIGH) c+=16;
  if(digitalRead(D3)==HIGH) c+=8;
  if(digitalRead(D2)==HIGH) c+=4;
  if(digitalRead(D1)==HIGH) c+=2;
  if(digitalRead(D0)==HIGH) c+=1;
  return c;
}

void wifiOutFunc() {
// this function is invoked when the TRS-80 has requested an OUT 240,X (so we are sending data *FROM* the TRS-80 *TO* the ESP-32)
    dataIn();
    Serial.printf("%d\n",getByte());
    resetWaitLatch();
}

void sendByte(unsigned char c) {
  if(c >= 128) {digitalWrite(D7,HIGH); c=c-128;} else {digitalWrite(D7,LOW);}
  if(c >= 64)  {digitalWrite(D6,HIGH); c=c-64 ;} else {digitalWrite(D6,LOW);}
  if(c >= 32)  {digitalWrite(D5,HIGH); c=c-32 ;} else {digitalWrite(D5,LOW);}
  if(c >= 16)  {digitalWrite(D4,HIGH); c=c-16 ;} else {digitalWrite(D4,LOW);}
  if(c >=  8)  {digitalWrite(D3,HIGH); c=c- 8 ;} else {digitalWrite(D3,LOW);}
  if(c >=  4)  {digitalWrite(D2,HIGH); c=c- 4 ;} else {digitalWrite(D2,LOW);}
  if(c >=  2)  {digitalWrite(D1,HIGH); c=c- 2 ;} else {digitalWrite(D1,LOW);}
  if(c >=  1)  {digitalWrite(D0,HIGH); c=c- 1 ;} else {digitalWrite(D0,LOW);}
}


/* jigger the flip flop that's tied to the WAIT* line on the TRS-80 to make sure it's in a known state */
void initFlipFlop() {
  digitalWrite(IC6A_PRE,LOW);
  digitalWrite(IC6A_CLR,LOW);

  digitalWrite(IC6A_PRE,HIGH);
  digitalWrite(IC6A_CLR,LOW);

  digitalWrite(IC6A_PRE,LOW);
  digitalWrite(IC6A_CLR,HIGH);

  digitalWrite(IC6A_PRE,HIGH);
  digitalWrite(IC6A_CLR,LOW);

  digitalWrite(IC6A_PRE,HIGH);
  digitalWrite(IC6A_CLR,HIGH);
}


inline void resetWaitLatch() {
  digitalWrite(IC6A_CLR,LOW);
  delayMicroseconds(1);
  digitalWrite(IC6A_CLR,HIGH);
  delayMicroseconds(1);
}

char x = 255;
void wifiInFunc() {
// this function is invoked when the TRS-80 has requested an INP() (so we are sending data *FROM* the ESP32 *TO* the TRS-80)
    dataOut();
    x--;   // just decrement a value every time this function is invoked until we have something more interesting to do
    sendByte(x);
    resetWaitLatch();
}

unsigned int iLEDCtr = 0;

void setup() {
    L1_BLACK();
    L2_BLACK();
    Serial.begin(115200);
    pinMode(2,OUTPUT);
    
    L2_RED();
    do {   // attempt to establish serial connection
      iLEDCtr++;
      delay(200);
      L2_YELLOW();
      digitalWrite(2,HIGH);
      delay(200);
      L2_RED();
      digitalWrite(2,LOW);
    }
    while(!Serial && iLEDCtr < 30);

    if(!Serial) {} else {
        L2_GREEN();
    }

    Serial.printf("Setting pin direction\n");  Serial.flush();
    setPinDirection();
    initFlipFlop();

    Serial.printf("Attaching WIFI_OUT interrupt\n");  Serial.flush();
    attachInterrupt(digitalPinToInterrupt(WIFI_OUT), wifiOutFunc, FALLING);
    Serial.printf("Attaching WIFI_IN interrupt\n");  Serial.flush();
    attachInterrupt(digitalPinToInterrupt(WIFI_IN), wifiInFunc, FALLING);

    L1_GREEN();
    Serial.printf("setup() complete.\n");  Serial.flush();
}

void loop() {
}
