SdFatSdioEX sdEx;

extern drivesettings Drives[4];


bool initSD() {
  if (!sdEx.begin()) {             // couldn't establish SD card connection
        p((char*)"ERROR:  Unable to open SdFatSdioEX object.\n");
        sdEx.initErrorHalt("SdFatSdioEX begin() failed");
        L2_RED();
        L1_RED();
        return false;
  }  
  return true;
}


void catalog() {
  File file;
  String workStrm;
  StringStream stream(workStrm); // set up string stream to capture SD card directory


  if(!initSD()) return;
    
  sdEx.chvol();
  sdEx.vwd()->rewind();
  while (file.openNext(sdEx.vwd(), O_RDONLY)) {
      if(!file.isHidden()) {
          file.printName(&stream);
          if (file.isDir()) {
           stream.write('/');
          }
          stream.flush();
          p((char*)"%s\n",workStrm.c_str());
          workStrm = "";
      }
      file.close();
  }
}


bool delFile(char* fileName) {
  if(!initSD()) return false;

  return sdEx.remove(fileName);
}

bool fileExists(char* fileName) {
  File file;
  if(!initSD()) return true;
  sdEx.chvol();

  if(file.open(fileName, O_RDONLY)) {
    file.close();
    return true;
  }
  return false;
}

void viewFile(char* fileName, bool bViewHex) {
  File file;
  char ch;

  if(!initSD()) return;
  sdEx.chvol();
  if(!file.open(fileName, O_RDONLY)) {
        p((char*)"ERROR:  Unable to open file: %s\n",fileName);
        L2_RED();
        L1_RED();
        return;
  }

  int iCtr = 1;
  while(file.available()) {
    if(file.read(&ch, 1) == 1) {
      if(bViewHex) {
        p((char*)"%02X ",ch);
        if(iCtr % 40 == 0) 
           p((char*)"\n");
      }
      else {
        p((char*)"%c",ch);
      }
      iCtr++;
    }
  }
  

  file.close();
}


void appendBufferToFile(char* buf, int iSize, char* fileName) {
  File file;
  
  if(!initSD()) return;

  sdEx.chvol();
  if(!file.open(fileName, O_CREAT | O_WRITE | O_AT_END)) {
        p((char*)"ERROR:  Unable to open file: %s\n",fileName);
        L2_RED();
        L1_RED();
        return;
  }

  if (iSize != file.write(buf, iSize)) {
        p((char*)"ERROR:  Unable to write to file: %s\n",fileName);
        L2_RED();
        L1_RED();
        file.close();
        return;    
  }

  file.close();
}


void openDiskFileByName(String sFileName, int iDriveNum) {
  File file;
  String workStrm;
  boolean bLoaded = false;
  
  StringStream stream(workStrm); // set up string stream to capture SD card directory

  if(!initSD()) return;

  sdEx.chvol();
  sdEx.vwd()->rewind();
  while (file.openNext(sdEx.vwd(), O_RDONLY)) {
      if(!file.isHidden()) {
          file.printName(&stream);
          if (file.isDir()) {
           stream.write('/');
          }
          stream.flush();
          if(workStrm == sFileName) {
             p((char*)"Opening disk %d file: ",iDriveNum);
             p((char*)workStrm.c_str());
             Drives[iDriveNum].diskFile = sdEx.open(workStrm.c_str(), FILE_READ);
             if(!Drives[iDriveNum].diskFile) {
                p((char*)"ERROR:  Unable to open file\n");
                L1_RED();
                L2_RED();
                file.close();
                return;
             }
             else {
              p((char*)"\n%d bytes in file.\n",Drives[iDriveNum].diskFile.available());
              bLoaded = true;
             }
          }
          workStrm = "";
      }
      file.close();
  }

  if(bLoaded == true)
      Drives[iDriveNum].sDiskFileName = sFileName;
  else
      p((char*)"ERROR: Unable to load file %s.\n",sFileName);

  return;
}
