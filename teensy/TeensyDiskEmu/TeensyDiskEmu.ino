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
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

#include "defines.h"


extern drivesettings Drives[4];
extern volatile int interruptStatus;


volatile int iBusDirection = 2;
volatile int dataBus;
volatile int address;



/* set up pin directionality */
void pinSetup() {
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

  pinSetup();                         // set pin modes
  initialPinState();                  // put pins in initial configuration
  



 
  L2_RED();
  while (!Serial && iLEDCtr < 30) {   // attempt to establish serial connection from Teensy
    iLEDCtr++;
    delay(100);
    L2_YELLOW();
    delay(100);
    L2_RED();
  }

  if (!Serial) {                      // couldn't establish serial connection within 6 seconds ((100+100)*30 milliseconds)
    L2_YELLOW();
  }
  else {                              // okay, serial established, let's move on
    L2_CYAN();
  }

  configureInterrupts();                             // tie interrupt lines to code blocks
  openDiskFileByName("NEWDOS_80sssd_jv1.DSK", 0);                // open file specified from SD card
  openDiskFileByName("more-arcade-1_80sssd_jv1.DSK", 1);                // open file specified from SD card
  openDiskFileByName("more-arcade-2_80sssd_jv1.DSK", 2);                // open file specified from SD card
  openDiskFileByName("more-arcade-3_80sssd_jv1.DSK", 3);                // open file specified from SD card

  p((char*)"\nReady.\n");
  if (!Serial) {
    L2_YELLOW();                     // unable to communicate over serial, show L2 as yellow
  }
  else {
    L2_GREEN();                      // all good to go
  }

  init1771Emulation();

  p((char*)"Setting up interval timer: %d", it.begin(clockTick, 25000));

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



void loop() {
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
