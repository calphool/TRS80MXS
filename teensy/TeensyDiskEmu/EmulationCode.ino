// type 1 status bits
#define BUSY 0x01
#define INDEXHOLE 0x02
#define TRACKZERO 0x04
#define CRCERR 0x08
#define SEEKERR 0x10
#define HEADENGAGED 0x20
#define WRITEPROT 0x40
#define NOTREADY 0x80

// read status bits
#define DRQ 0x02
#define LOSTDATA 0x04
#define NOTFND 0x10
#define FA 0x20
#define F9 0x40
#define F8 0x60
#define FB 0x00

// write status bits
#define WRITEFAULT 0x20


#define CMD_RESTORE 0x00
#define CMD_SEEK    0x10
#define CMD_STEP    0x20
#define CMD_STEPIN  0x40
#define CMD_STEPOUT 0x60

#define CMD_READ    0x80
#define CMD_WRITE   0x90

#define CMD_READ_ADDR 0xc4
#define CMD_READ_TRK  0xe4
#define CMD_WRITE_TRK 0xf4

#define CMD_FORCE_INTERRUPT 0xd0

#define CMD_RESTORE_MASK 0xf0
#define CMD_SEEK_MASK 0xf0
#define CMD_STEP_MASK 0xe0
#define CMD_STEPIN_MASK 0xe0
#define CMD_STEPOUT_MASK 0xe0
#define CMD_READ_MASK 0xe0
#define CMD_WRITE_MASK 0xe0
#define CMD_READ_ADDR_MASK 0xff
#define CMD_READ_TRK_MASK 0xfe
#define CMD_WRITE_TRK_MASK 0xff
#define CMD_FORCE_INTERRUPT_MASK 0xf0

volatile int currentDrive = 0x0;
volatile int interruptStatus;
volatile int oldInterruptStatus;



drivesettings Drives[4];




void init1771Emulation() {
  Drives[0].statusRegister = NOTREADY | TRACKZERO;
  Drives[1].statusRegister = NOTREADY | TRACKZERO;
  Drives[2].statusRegister = NOTREADY | TRACKZERO;
  Drives[3].statusRegister = NOTREADY | TRACKZERO;
}


inline void cmdRestore() {
    p((char*)"    RESTORE CMD\n");
    Drives[currentDrive].trackNum = 0;
    Drives[currentDrive].diskFile.seek(0);
    Drives[currentDrive].statusRegister = HEADENGAGED | TRACKZERO;
    Drives[currentDrive].currentCommand = CMD_RESTORE;
    interruptStatus |= (0x40);
    L2_BLUE();
}

inline void cmdSeek() {
    p((char*)"    SEEK CMD\n");
    Drives[currentDrive].trackNum = Drives[currentDrive].dataRegister;
    Drives[currentDrive].diskFile.seek(Drives[currentDrive].trackNum * 2560 + Drives[currentDrive].sectorNum * 256);
    Drives[currentDrive].statusRegister = HEADENGAGED | BUSY;
    if(Drives[currentDrive].trackNum == 0)
        Drives[currentDrive].statusRegister |= TRACKZERO;
    Drives[currentDrive].busyCtr = 79;
    Drives[currentDrive].currentCommand = CMD_SEEK;   
    L2_YELLOW(); 
}

inline void cmdStep() {
    p((char*)"    STEP CMD\n");
    if((Drives[currentDrive].commandRegister & 0x10) == 0x10) {            // only do this stuff if the update flag is set on the track register, otherwise just generate an interupt like we complied
      if(Drives[currentDrive].iTrackDirection == OUT) {
          if(Drives[currentDrive].trackNum > 0)
              Drives[currentDrive].trackNum--;
      }
      else {
        Drives[currentDrive].trackNum++;
      }
    }
    Drives[currentDrive].diskFile.seek(Drives[currentDrive].trackNum * 2560 + Drives[currentDrive].sectorNum * 256);
    Drives[currentDrive].statusRegister = INDEXHOLE;  
    Drives[currentDrive].currentCommand = CMD_STEP;  
    L2_CYAN();
}


inline void cmdStepIn() {
    p((char*)"    STEP IN CMD\n");
    if((Drives[currentDrive].commandRegister & 0x10) == 0x10) {           // only do this stuff if the update flag is set on the track register, otherwise just generate an interupt like we complied
      Drives[currentDrive].trackNum++;
    }
    Drives[currentDrive].iTrackDirection = IN; // IN is away from 0
    Drives[currentDrive].diskFile.seek(Drives[currentDrive].trackNum * 2560 + Drives[currentDrive].sectorNum * 256);
    Drives[currentDrive].statusRegister = INDEXHOLE;
    Drives[currentDrive].currentCommand = CMD_STEPIN;
    L2_GREEN();
}


inline void cmdStepOut() {  
    p((char*)"    STEP OUT CMD\n");
    if((Drives[currentDrive].commandRegister & 0x10) == 0x10) {           // only do this stuff if the update flag is set on the track register, otherwise just generate an interupt like we complied
      if(Drives[currentDrive].trackNum > 0) 
         Drives[currentDrive].trackNum--;
    }
    Drives[currentDrive].iTrackDirection = OUT;                         // OUT is toward 0
    Drives[currentDrive].diskFile.seek(Drives[currentDrive].trackNum * 2560 + Drives[currentDrive].sectorNum * 256);
    Drives[currentDrive].statusRegister = INDEXHOLE;
    Drives[currentDrive].currentCommand = CMD_STEPOUT;
    L2_WHITE();
}

inline void cmdRead() {
    Drives[currentDrive].byteCtr = 256;
    Drives[currentDrive].busyCtr = 256;
    Drives[currentDrive].afterSectorBusyCtr = 5;
    p((char*)"    READ SECTOR CMD\n");
    Drives[currentDrive].statusRegister = BUSY;
    Drives[currentDrive].currentCommand = CMD_READ;
    Drives[currentDrive].DRQCtr = 0;
    if(Drives[currentDrive].sectorsRead == 0)
       Drives[currentDrive].DRQCtr = 4;
    else 
       Drives[currentDrive].statusRegister |= DRQ;

    Drives[currentDrive].sectorsRead++;
    L2_VIOLET();
}

inline void cmdForceInterrupt(int interruptType) {
    p((char*)"    FORCE INTERRUPT\n");
    Drives[currentDrive].statusRegister = HEADENGAGED;
    if(Drives[currentDrive].trackNum == 0)
        Drives[currentDrive].statusRegister |= TRACKZERO;

    if(interruptType != 0x00) {
      p((char*)"Special interrupt requested! %x",interruptType);
    }

    Drives[currentDrive].currentCommand = CMD_FORCE_INTERRUPT;
    L2_BLACK();
}


inline void cmdWrite() {
  p((char*)"    WRITE CMD (unimpl)\n");
  Drives[currentDrive].currentCommand = CMD_WRITE;
}

inline void cmdReadAddr() {
   p((char*)"    READ ADDR (unimpl)\n");
   Drives[currentDrive].currentCommand = CMD_READ_ADDR;
}

inline void cmdReadTrk() {
  p((char*)"    READ TRK (unimpl)\n"); 
  Drives[currentDrive].currentCommand = CMD_READ_TRK ;
}

inline void cmdWriteTrk() {
  p((char*)"    WRITE TRK (unimpl)\n");
  Drives[currentDrive].currentCommand = CMD_WRITE_TRK;
}


void invokeCommand() {
    p((char*)"<::command reg::> \n");
    Drives[currentDrive].commandRegister = dataBus;
  
  //
  // Command   Type    76543210
  //
  // RESTORE     1     0000XXXX
  // SEEK        1     0001XXXX
  // STEP        1     001uXXXX    u = update track register
  // STEP IN     1     010uXXXX
  // STEP OUT    1     011uXXXX
  // READ CMD    2     100mbXXX    m = muliple record, b = 128-1024 block length (0) or 16-4096 block length(1)
  // WRITE CMD   2     101mbXXX
  // READ ADDR   3     11000100
  // READ TRK    3     1110010X
  // WRITE TRK   3     11110100
  // FORCE INTRP 4     1101XXXX
  
  if((Drives[currentDrive].commandRegister & CMD_RESTORE_MASK) == CMD_RESTORE) {              // restore command
    cmdRestore();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_READ_MASK) == CMD_READ) {        // read command
    cmdRead();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_FORCE_INTERRUPT_MASK) == CMD_FORCE_INTERRUPT) {        // force interrupt command
     cmdForceInterrupt(Drives[currentDrive].commandRegister & 0x0f);
     return;
  }
  if((Drives[currentDrive].commandRegister & CMD_SEEK_MASK) == CMD_SEEK) {         // seek command
    cmdSeek();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_STEP_MASK) == CMD_STEP) {         // step command
    cmdStep();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_STEPIN_MASK) == CMD_STEPIN) {        // step in command
    cmdStepIn();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_STEPOUT_MASK) == CMD_STEPOUT) {        // step out command
    cmdStepOut();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_WRITE_MASK) == CMD_WRITE) {        // write command
    cmdWrite();
    return; 
  }
  if((Drives[currentDrive].commandRegister & CMD_READ_ADDR_MASK) == CMD_READ_ADDR) {               // read address command
    cmdReadAddr();
    return;
  }
  if((Drives[currentDrive].commandRegister & CMD_READ_TRK_MASK) == CMD_READ_TRK) {        // read track command
    cmdReadTrk();
    return;      
  }
  if((Drives[currentDrive].commandRegister & CMD_WRITE_TRK_MASK) == CMD_WRITE_TRK) {               // write track command
     cmdWriteTrk();
     return;
  }

  Drives[currentDrive].currentCommand = 0;
  p((char*)"    UNKNOWN COMMAND\n");
  //digitalWriteFast(INTERUPT_TO_TRS80, LOW);
  //p((char*)" (irq=1) ");
}


inline void driveSelect() {
    p((char*)"<::drive select::> \n");
    if(dataBus == 0x01)
      currentDrive = 0;
    else if(dataBus == 0x02)
      currentDrive = 1;
    else if(dataBus == 0x04)
      currentDrive = 2;
    else if(dataBus == 0x08)
      currentDrive = 3;
    else {
      p((char*)" ??Received nonsense value for drive number, assuming 0??\n");
      currentDrive = 0;
    }
    Drives[currentDrive].statusRegister |= HEADENGAGED;
    if(Drives[currentDrive].trackNum == 0) {
      Drives[currentDrive].statusRegister |= TRACKZERO;
    }
    Drives[currentDrive].motorRunningCtr = 80;

    if(Drives[currentDrive].currentCommand == CMD_SEEK && Drives[currentDrive].busyCtr > 8) { // hack to make seeks late in the cycle match
      Drives[currentDrive].busyCtr = 8;
    }
}

inline void setTrkReg() {
    p((char*)"<::track reg::> \n");
    Drives[currentDrive].trackNum = dataBus;  // track
    Drives[currentDrive].diskFile.seek(Drives[currentDrive].trackNum * 2560 + Drives[currentDrive].sectorNum * 256);
    //statusRegister = INDEXHOLE;
    return;
}

inline void setSectorReg() {
    p((char*)"<::sector reg::> \n");
    Drives[currentDrive].sectorNum = dataBus; // sector
    Drives[currentDrive].diskFile.seek(Drives[currentDrive].trackNum * 2560 + Drives[currentDrive].sectorNum * 256);
    //statusRegister = INDEXHOLE;
    return;
}

inline void setDataReg() {
    p((char*)"<::data reg::> \n");
    Drives[currentDrive].dataRegister = dataBus;  // data byte
    //statusRegister = INDEXHOLE;
    return;
}


/* ***********************************************************************
 *  Data pushes from TRS-80
 *  **********************************************************************
 */
void PokeFromTRS80() {
  p((char*)"%d.[%02X,%02X] :---> (0x%02X)  ---> 0x%04X ",/*sectorsRead*/0, Drives[currentDrive].trackNum,Drives[currentDrive].sectorNum,dataBus, address);  
  
  if((address & 0xfffc) == 0x37e0) { // drive select
     return driveSelect();
  } 
  if(address == 0x37ec) { // command invokation request
    return invokeCommand();
  }
  if(address == 0x37ed) { // set track register
    return setTrkReg();
  }
  if(address == 0x37ee) { // set sector register
    return setSectorReg();
  } 
  if(address == 0x37ef) {  // set data register
    return setDataReg();
  }

  p((char*)"Unhandled POKE!\n\n");
}


inline int getStatusRegister() {
    Drives[currentDrive].statusRegisterChecks++;
    if(Drives[currentDrive].DRQCtr > 0) {
      Drives[currentDrive].DRQCtr--;
    }
    if(Drives[currentDrive].busyCtr > 0) {
      Drives[currentDrive].busyCtr--;
    }

    if(Drives[currentDrive].currentCommand == CMD_READ) {
      if(Drives[currentDrive].DRQCtr == 0) 
          Drives[currentDrive].statusRegister |= DRQ;
      
      if(Drives[currentDrive].byteCtr == 0) {
          Drives[currentDrive].statusRegister &= ~(DRQ);
          interruptStatus |= (0x40);
      }
    }

    if(Drives[currentDrive].currentCommand == CMD_SEEK) {
      if(Drives[currentDrive].busyCtr <= 0) {
          interruptStatus |= (0x40);
          //digitalWriteFast(INTERUPT_TO_TRS80, LOW);
          //p((char*)" (irq=1) ");
      }
    }

    if(Drives[currentDrive].busyCtr == 0) {
      Drives[currentDrive].afterSectorBusyCtr--;
      if(Drives[currentDrive].afterSectorBusyCtr <= 0) { 
         Drives[currentDrive].statusRegister &= ~(BUSY);
         //digitalWriteFast(INTERUPT_TO_TRS80, LOW);
         //p((char*)" (irq=1) ");
      }
    }

    if(Drives[currentDrive].motorRunningCtr == 0)
       Drives[currentDrive].statusRegister |= NOTREADY;
    else
       Drives[currentDrive].statusRegister &= ~(NOTREADY);

    if(Drives[currentDrive].sectorsRead == 0) {
      if(Drives[currentDrive].trackNum == 0) {
        Drives[currentDrive].statusRegister |= TRACKZERO; // force track number to zero on the first status check
      }
    }

    if(Drives[currentDrive].trackNum == 17) {
      Drives[currentDrive].statusRegister |= 0x20;   // goofy thing that's required because NEWDOS expects to see "FA" status bits on track 17... is this what was keeping me from booting?
    }

    if(Drives[currentDrive].statusRegisterChecks % 1200 == 0) {
      if((Drives[currentDrive].statusRegister & INDEXHOLE) == INDEXHOLE)
        Drives[currentDrive].statusRegister &= ~(INDEXHOLE);
      else
        Drives[currentDrive].statusRegister |= INDEXHOLE;
    }
    
    //p((char*)" <--- (0x%02X) <::status reg::>\n",Drives[currentDrive].statusRegister);
    
    //digitalWriteFast(INTERUPT_TO_TRS80, HIGH);
    return Drives[currentDrive].statusRegister;
}

inline int getDataRegister() {
    if(Drives[currentDrive].byteCtr > 0) {
       Drives[currentDrive].byteCtr--;
       if(Drives[currentDrive].byteCtr == 0) {
          Drives[currentDrive].busyCtr = 3;
       }
       Drives[currentDrive].dataRegister = Drives[currentDrive].diskFile.read();
    }
    
    p((char*)" <--- (0x%02X) <::data reg::> \n", Drives[currentDrive].dataRegister);
    return Drives[currentDrive].dataRegister;
}


/*****************************************************************
 * Read Requests from TRS-80
 * ***************************************************************
 */
int PeekFromTRS80() {
  //p((char*)"%d.[%02X,%02X] <---: 0x%04X ",/*sectorsRead*/0,Drives[currentDrive].trackNum,Drives[currentDrive].sectorNum,address); 

  if(address == 0x37ec) {             // read status register
    interruptStatus &= ~(0x40);
    return getStatusRegister();
  }

  if(address == 0x37ef) {             // read data register
    return getDataRegister();
  }

  if(address == 0x37ee) {              // read sector register
    p((char*)" <--- (0x%02X) <::sector reg::>\n", Drives[currentDrive].sectorNum);
    return Drives[currentDrive].sectorNum;
  }

  if(address == 0x37ed) {              // read track register
    p((char*)" <--- (0x%02X) <::track reg::>\n", Drives[currentDrive].trackNum);
    return Drives[currentDrive].trackNum;
  }  

  if(address == 0x37e0) {             // read interrupt latch (supposed to reset the latch)       
    if((oldInterruptStatus & 0x80) == 0x80) {
        digitalWriteFast(INTERUPT_TO_TRS80,HIGH);
        interruptStatus = 0;      
    }
 
    //p((char*)" <--- (0x%02X) <::interrupt latch::>\n",interruptStatus);

    oldInterruptStatus = interruptStatus;
    return interruptStatus;
  }

  p((char*)" <--- (0xfe) <::HUH Why Am I Here?::>\n");
  return 0xfe;
}
