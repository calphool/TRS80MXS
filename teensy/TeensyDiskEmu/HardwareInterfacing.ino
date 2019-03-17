
inline void dataBusOutFromTeensyMode() {
  if(iBusDirection == OUT) return;
  
  pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);  
  iBusDirection = OUT;
}



inline void dataBusInToTeensyMode() {
  if(iBusDirection == IN) return;

  pinMode(D0,INPUT);
  pinMode(D1,INPUT);
  pinMode(D2,INPUT);
  pinMode(D3,INPUT);
  pinMode(D4,INPUT);
  pinMode(D5,INPUT);
  pinMode(D6,INPUT);
  pinMode(D7,INPUT);  
  
  iBusDirection = IN;
}



/* jigger the flip flop that's tied to the WAIT* line on the TRS-80 to make sure it's in a known state */
void initFlipFlop() {
  digitalWrite(FF_PRE,LOW);
  digitalWrite(FF_CLR,LOW);

  digitalWrite(FF_PRE,HIGH);
  digitalWrite(FF_CLR,LOW);

  digitalWrite(FF_PRE,LOW);
  digitalWrite(FF_CLR,HIGH);

  digitalWrite(FF_PRE,HIGH);
  digitalWrite(FF_CLR,LOW);

  digitalWrite(FF_PRE,HIGH);
  digitalWrite(FF_CLR,HIGH);
}



/* reset the latch that's tied to TRS-80 WAIT* line */
inline void resetWaitLatch() {
  digitalWriteFast(FF_CLR,LOW);
  delayMicroseconds(1);
  digitalWriteFast(FF_CLR,HIGH);
  delayMicroseconds(1);
}



inline int getDataBusValue() {
  dataBusInToTeensyMode();
  return (GPIOC_PDIR & 0x00ff);
}



/* set the value of the data bus */
inline void setDataBus(int b) {
  dataBusOutFromTeensyMode();
  if(b >= 128) {digitalWriteFast(D7,HIGH); b=b-128;} else {digitalWriteFast(D7,LOW);}
  if(b >= 64)  {digitalWriteFast(D6,HIGH); b=b-64; } else {digitalWriteFast(D6,LOW);}
  if(b >= 32)  {digitalWriteFast(D5,HIGH); b=b-32; } else {digitalWriteFast(D5,LOW);}
  if(b >= 16)  {digitalWriteFast(D4,HIGH); b=b-16; } else {digitalWriteFast(D4,LOW);}
  if(b >= 8)   {digitalWriteFast(D3,HIGH); b=b-8;  } else {digitalWriteFast(D3,LOW);}
  if(b >= 4)   {digitalWriteFast(D2,HIGH); b=b-4;  } else {digitalWriteFast(D2,LOW);}
  if(b >= 2)   {digitalWriteFast(D1,HIGH); b=b-2;  } else {digitalWriteFast(D1,LOW);}
  if(b >= 1)   {digitalWriteFast(D0,HIGH); b=b-1;  } else {digitalWriteFast(D0,LOW);}
}
