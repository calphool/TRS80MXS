//for use with TRS80MSX drive emulation shield v1.0

#include <BlockDriver.h>
#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <sdios.h>
#include <SysCall.h>
#include <StringStream.h>
#include <stdarg.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#include "defines.h"

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

extern drivesettings Drives[4];
extern volatile int interruptStatus;


volatile int iBusDirection = 2;
volatile int dataBus;
volatile int address;

char respBuffer[255];
char workBuffer[255];

byte disk_spin_0[8] = {
  B11111,
  B11001,
  B11101,
  B11101,
  B11111,
  B11011,
  B11111,
};  
byte disk_spin_1[8] = {
  B11111,
  B11111,
  B11101,
  B11101,
  B11001,
  B11011,
  B11111,
};  
byte disk_spin_2[8] = {
  B11111,
  B11111,
  B10111,
  B10111,
  B10011,
  B11011,
  B11111,
};  
byte disk_spin_3[8] = {
  B11111,
  B10011,
  B10111,
  B10111,
  B11111,
  B11011,
  B11111,
};  



/* set up pin directionality */
void pinSetup() {
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_EN, OUTPUT);
  pinMode(LCD_D4, OUTPUT);
  pinMode(LCD_D5, OUTPUT);
  pinMode(LCD_D6, OUTPUT);
  pinMode(LCD_D7, OUTPUT);
  pinMode(LED2_RED, OUTPUT);
  pinMode(LED2_GREEN, OUTPUT);
  pinMode(LED2_BLUE, OUTPUT);
  pinMode(INTERUPT_TO_TRS80, OUTPUT);
  pinMode(FF_CLR, OUTPUT);
  pinMode(FF_PRE, OUTPUT);
  
  pinMode(_37E0RD, INPUT);
  pinMode(_37E0WR, INPUT);
  pinMode(_37E4RD, INPUT);
  pinMode(_37E4WR, INPUT);
  pinMode(_37E8RD, INPUT);
  pinMode(_37E8WR, INPUT);
  pinMode(_37ECRD, INPUT);
  pinMode(_37ECWR, INPUT);
  pinMode(_A0, INPUT);
  pinMode(_A1, INPUT);
  
  pinMode(NOTHING1, OUTPUT);
  pinMode(NOTHING2, OUTPUT);
  pinMode(SYSRES, INPUT);
  dataBusOutFromTeensyMode();
  dataBusInToTeensyMode();
}


/* initialize output pins */
void initialPinState() {
  digitalWriteFast(INTERUPT_TO_TRS80, HIGH); // turn off TRS-80 interrupt line (it's active low)
  initFlipFlop();                            // jigger flip flop into a known state (loaded, ready for trigger)
}

IntervalTimer it;

void _37E0WRInterrupt() {
  dataBus = getDataBusValue();
  address = 0x37e0 + ((GPIOC_PDIR & 0x0300) >> 8);
  PokeFromTRS80();
  resetWaitLatch();
}

void _37E8WRInterrupt() {
  dataBus = getDataBusValue();
  address = 0x37e8 + ((GPIOC_PDIR & 0x0300) >> 8);
  PokeFromTRS80();
  resetWaitLatch();
}

void _37E4WRInterrupt() {
  dataBus = getDataBusValue();
  address = 0x37e4 + ((GPIOC_PDIR & 0x0300) >> 8);
  PokeFromTRS80();
  resetWaitLatch();
}

void _37ECWRInterrupt() {
  dataBus = getDataBusValue();
  address = 0x37ec + ((GPIOC_PDIR & 0x0300) >> 8);
  PokeFromTRS80();
  resetWaitLatch();
}


void _37ECRDInterrupt() {
  address = 0x37ec + ((GPIOC_PDIR & 0x0300) >> 8);
  setDataBus(PeekFromTRS80());
  resetWaitLatch();
}

void _37E8RDInterrupt() {
  address = 0x37e8 + ((GPIOC_PDIR & 0x0300) >> 8);
  setDataBus(PeekFromTRS80());
  resetWaitLatch();
}

void _37E4RDInterrupt() {
  address = 0x37e4 + ((GPIOC_PDIR & 0x0300) >> 8);
  setDataBus(PeekFromTRS80());
  resetWaitLatch();
}

void _37E0RDInterrupt() {
  address = 0x37e0 + ((GPIOC_PDIR & 0x0300) >> 8);
  setDataBus(PeekFromTRS80());
  resetWaitLatch();
}



/* wire up the interrupts */
void configureInterrupts() {
  attachInterrupt(digitalPinToInterrupt(_37ECRD), _37ECRDInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(_37E8RD), _37E8RDInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(_37E4RD), _37E4RDInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(_37E0RD), _37E0RDInterrupt, FALLING);

  attachInterrupt(digitalPinToInterrupt(_37ECWR), _37ECWRInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(_37E8WR), _37E8WRInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(_37E4WR), _37E4WRInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(_37E0WR), _37E0WRInterrupt, FALLING);
}


/* initialize everything */
void setup() {
  int iLEDCtr = 0;

  cli();                              // turn off interrupts during setup()

  Serial.begin(2000000);              // high speed serial over USB
  //  display.begin(SSD1306_SWITCHCAPVCC);
  //  display.display();
  //  delay(1000);
  //  display.clearDisplay();
  //  display.drawPixel(10,10,WHITE);
  //  display.display();
  //  delay(1000);

  lcd.createChar(0, disk_spin_0);
  lcd.createChar(1, disk_spin_1);
  lcd.createChar(2, disk_spin_2);
  lcd.createChar(3, disk_spin_3);

  pinSetup();                         // set pin modes
  initialPinState();                  // put pins in initial configuration  

  strcpy(respBuffer, "PENDING\n");
  workBuffer[0] = 0x0;

  lcd.begin(20,4);
  lcd.clear();


  lcd.setCursor(0,0);
  lcd.print("Looking for serial.");
  L2_RED();
  while (!Serial && iLEDCtr < 30) {   // attempt to establish serial connection from Teensy
    iLEDCtr++;
    delay(100);
    L2_YELLOW();
    delay(100);
    L2_RED();
  }

  lcd.clear();
  lcd.setCursor(0,0);

  if (!Serial) {                      // couldn't establish serial connection within 6 seconds ((100+100)*30 milliseconds)
    L2_YELLOW();
    lcd.print("Serial not connected");
  }
  else {                              // okay, serial established, let's move on
    lcd.print("Serial found");
    L2_CYAN();
  }

  lcd.setCursor(0,1);
  lcd.print("Loading deflt drives");

  configureInterrupts();                             // tie interrupt lines to code blocks
  openDiskFileByName("newdos80.dsk", 0);                // open file specified from SD card
  openDiskFileByName("more-arcade-1_80sssd_jv1.DSK", 1);                // open file specified from SD card
  openDiskFileByName("blank.DSK", 2);                // open file specified from SD card
  openDiskFileByName("trs80mxs.dsk",3);

  p((char*)"\nReady.\n");
  if (!Serial) {
    L2_YELLOW();                     // unable to communicate over serial, show L2 as yellow
  }
  else {
    L2_GREEN();                      // all good to go
  }


  lcd.setCursor(0,2);
  lcd.print("Turning on timer");

  init1771Emulation();

  p((char*)"Setting up interval timer: %d", it.begin(clockTick, 25000));

  lcd.clear();
  sei();                             // enable interrupts
}


void clockTick() {
  // comment out if you want to turn off the clock functionality...

  interruptStatus = interruptStatus | 0x80;
  digitalWriteFast(INTERUPT_TO_TRS80, LOW);
}


String sCommand;
String sWorkBuffer;
int dNum;
boolean bInsideUploadCommand = false;
String sFileName;

void handleUploadProcess() {
  if (bInsideUploadCommand == false) {
    // we are preparing to upload a file... get ready to write file
    sFileName = sCommand.substring(strlen("upload ")).trim();
    sWorkBuffer = "";
    if (fileExists((char*)sFileName.c_str()) )
      fatalError((char*)"ERROR: file already exists.");
    else {
      p((char*)"ready\n\n");
      L2_GREEN();
      dNum = 0;
      bInsideUploadCommand = true;
    }
    return;
  }
  else {
    if (sCommand.indexOf("done ") == 0) {
      bInsideUploadCommand = false;
      //TODO: add checksum code here...
      p((char*)"ok\n\n");
      return;
    }
    if (sCommand.indexOf("chunk ") == 0) {
      sCommand = sCommand.substring(6).trim();
      String chkSum = sCommand.substring(0, 8);
      sCommand = sCommand.substring(8).trim();
      sWorkBuffer = sCommand;
    }
    else {
      sWorkBuffer += sCommand;
    }

    if (sWorkBuffer.indexOf("$$$") == -1) return;

    sWorkBuffer = sWorkBuffer.substring(0, sWorkBuffer.length() - 3);

    size_t buflen;
    sWorkBuffer = sWorkBuffer.replace("~", "\n");
    char* buf = base64Decode((char*)sWorkBuffer.c_str(), sWorkBuffer.length(), &buflen);
    if (buf == NULL) {
      fatalError((char*)"ERROR: problem decoding chunk\n\n");
      bInsideUploadCommand = false;
      return;
    }
    dNum++;

    //TODO:  add checksum code here...
    if (buflen > 0) {
      //p("%d ",buflen);
      appendBufferToFile(buf, buflen, (char*)sFileName.c_str());
      if (dNum % 2 == 0) {
        L2_GREEN();
      }
      else {
        L2_YELLOW();
      }
    }
    else {
      L2_BLACK();
    }

    free(buf);

    p((char*)"ready\n\n");
  }
}


void updateLCD() {
  static int x = 0;
  static char buffer[80];
  static unsigned long bSpinning[4];
  static unsigned long temp;
      
  for(int i=0;i<4;i++) {
    lcd.setCursor(0,i);
    lcd.print(i);
        
    if((Drives[i].statusRegister & 0x01) == 0x01) 
        bSpinning[i] = millis();
    
    lcd.setCursor(2,i);
    strcpy(buffer,Drives[i].sDiskFileName.c_str()); 
    buffer[18]=0x0;
    while(strlen(buffer) < 18) strcat(buffer," ");
    lcd.print(buffer);
  }

  for(int i=0;i<4;i++) {
    temp = millis() - bSpinning[i];
    if(temp > 1000)
        if((Drives[i].statusRegister & 0x01) != 0x01)
            bSpinning[i] = 0;
            
    temp = temp % 1000;
    lcd.setCursor(1,i);
    if(temp < 250)
      lcd.write(byte(0));
    else if(temp < 500)
      lcd.write(byte(1));
    else if(temp < 750)
      lcd.write(byte(2));
    else
      lcd.write(byte(3));

    if(bSpinning[i] == 0) {
        lcd.setCursor(1,i);     
        lcd.print(" "); 
    }
  }


}


int lpCtr = 0;

void loop() {  
  lpCtr++;

  if(lpCtr == 32767) {
    lpCtr = 0;
    updateLCD();
  }

  while (Serial.available() > 0 ) {
    sCommand = Serial.readString().trim();
    if (!bInsideUploadCommand)
      p((char*)"\nReceived command:  >>%s<<\n\n", sCommand.c_str());

    if (sCommand.indexOf("mount ") != -1) {
      sCommand = sCommand.substring(sCommand.indexOf("mount ") + 6);
      dNum = sCommand.substring(0, 2).toInt();
      sCommand = sCommand.substring(2);
      p((char*)"Mounting disk image file: >>%s<< on drive %d\n", sCommand.c_str(), dNum);
      openDiskFileByName(sCommand, dNum);
      init1771Emulation();
      p((char*)"\n");
    }

    if (sCommand.indexOf("catalog") != -1) {
      catalog();
      p((char*)"\n\n");
    }

    if (sCommand.indexOf("show mounts") != -1) {
      for (int i = 0; i < 4; i++) {
        p((char*)"%d. %s\n", i, Drives[i].sDiskFileName.c_str());
      }
      p((char*)"\n");
    }

    if (sCommand.indexOf("rm ") == 0) {
      sCommand = sCommand.substring(3);
      if (delFile((char*)sCommand.c_str()))
        p((char*)"ok\n\n");
      else
        p((char*)"ERROR: file not deleted\n\n");
    }

    if (sCommand.indexOf("view ") == 0) {
      sCommand = sCommand.substring(5);
      viewFile((char*)sCommand.c_str(), false);
      p((char*)"\n\n");
    }

    if (sCommand.indexOf("viewhex ") == 0) {
      sCommand = sCommand.substring(8);
      viewFile((char*)sCommand.c_str(), true);
      p((char*)"\n\n");
    }

    if (sCommand.indexOf("help") != -1) {
      p((char*)"mount\n");
      p((char*)"show mounts\n");
      p((char*)"catalog\n");
      p((char*)"upload\n");
      p((char*)"rm\n");
      p((char*)"view\n");
      p((char*)"viewhex\n");
      p((char*)"help\n");
      p((char*)"\n");
    }

    if (sCommand.indexOf("upload") != -1 || bInsideUploadCommand == true) {
      handleUploadProcess();
    }
  }
}
