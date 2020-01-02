#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
 
char workBuffer[255];
char tempBuffer[64];

#define COMMUNICATION_ADDRESS 0x37e8
#define LINEFEED 0x0a
#define CARRIAGERETURN 0x0d


// trim spaces from the front and back of the passed in string
char * trim_space(char *str) __z88dk_fastcall {
    char *end;
    while (isspace(*str)) {
        str = str + 1;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end = end - 1;
    }
    *(end+1) = '\0';
    return str;
}


// send command to the TRS80 communication address one byte at a time followed by a linefeed character
void sendCommand(char* s) __z88dk_fastcall  {
   char c;
   for(int i=0;i<strlen(s);i++) {
   	  c = *(s+i);
   	  memset(COMMUNICATION_ADDRESS, c, 1);
   }
   memset(COMMUNICATION_ADDRESS,LINEFEED,1);
}


// get one byte from the communication address
char getByteFromTeensy() __z88dk_fastcall  {
	char* c = COMMUNICATION_ADDRESS;
	return (*c);
}


// get string response from Teensy one byte at a time (using getByteFromTeensy()), putting it into the work buffer 
// until a carriage return or linefeed.  Returns length of string in workBuffer
int getRespString() __z88dk_fastcall  {
	workBuffer[0] = 0x0;
    int ctr = 0;
    char c;

    do {
        c = getByteFromTeensy();
        if(c != LINEFEED && c != CARRIAGERETURN) {
        	workBuffer[ctr] = c;
        	workBuffer[ctr+1] = 0x0;
        	ctr++;
        	if(ctr > 254)
        		return -1;
        }
    }
    while(c != LINEFEED && c != CARRIAGERETURN);

    return strlen(workBuffer);
}


// read bytes from Teensy without capturing string into a work buffer (used to "eat" data we don't need that the Teensy wants to send)
int readTeensyRespUntilCR() __z88dk_fastcall  {
	int ctr = 0;
  char c;
	do {
       c = getByteFromTeensy();
       ctr++;
       if(ctr > 128)    return -1;
	}
	while(c != LINEFEED && c != CARRIAGERETURN);

	return 0;
}


void mount(int iDriveNum, char* driveImageName) {
	if(readTeensyRespUntilCR() == -1) {
		printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
		return;
	}

  if(strlen(driveImageName) < 1) {
     printf("%s IS NOT A VALID DRIVE IMAGE NAME.\n", driveImageName);
     return;
  }

  sprintf(tempBuffer, "MOUNT %d %s", iDriveNum, driveImageName);
  sendCommand(tempBuffer);

  if(getRespString() < 0) {
		printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
		return;    	
  }
  readTeensyRespUntilCR();
  getRespString();
  printf("\n\n\n%s\n\n\n", strupr(workBuffer));
}


void images(char* sPrefix) __z88dk_fastcall {
  char buff[128];
  int ctr = 0;

  buff[0] = 0x0;
  strcpy(buff,sPrefix);

  if(strlen(buff) > 0) {
      strcpy(buff, trim_space(buff));
      for(int i=0;i<strlen(buff);i++)
          buff[i] = toupper(buff[i]);
  }

	strcpy(tempBuffer, "IMGCOUNT");
	sendCommand(tempBuffer);
	if(readTeensyRespUntilCR() == -1) {
		printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
		return;
	}
  getRespString(); // workBuffer should contain a string with the number of disk images found

  int i = atoi(workBuffer);
  printf("\n>> %d IMAGES AVAILABLE\n>>SEARCHING...\n", i);

  for(int x = 0; x < i; x++) {
    sprintf(tempBuffer,"IMGAT %d", x);
    sendCommand(tempBuffer);
	  if(readTeensyRespUntilCR() == -1) {
			printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
			return;
	  }
    getRespString();
    strcpy(workBuffer, strupr(workBuffer));
    if(strlen(buff) == 0 || (strstr(workBuffer,buff) != NULL))  // if no "contains" string was passed, or we found the matching string then print
      printf("%s\n", workBuffer);	   
      ctr++;
  }
  if(ctr == 0 && strlen(buff) > 0) 
    printf(">>NO IMAGES FOUND THAT CONTAIN '%s'.", buff);
  
  printf("\n\n");
}


void listmounts() __z88dk_fastcall {
  int x = 0;

	strcpy(tempBuffer, "LISTMOUNTS");
	sendCommand(tempBuffer);

  if(readTeensyRespUntilCR() == -1) {
		printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
		return;
  }

  printf("\n\n");
  getRespString();
  printf("%d> ", x);
  for(int i=0;i<strlen(workBuffer);i++) {
    char c = *(workBuffer+i);
    if(c == ',') {
      x++;
    	printf("\n%d> ", x);
    }
    else
    	printf("%c", toupper(c));
  }	   
  printf("\n");
}


void cls() __z88dk_fastcall {
  // ROM routine for clearing screen
  #asm
      push af
      call 0x1c9
      pop af
  #endasm
}

char stringIsNumeric(char* s) __z88dk_fastcall {
  for(int i=0;i<strlen(s);i++) {
    if(!isdigit(*(s+i)))
      return 0;
  }
  return 1;
}


int main(int argc, char *argv[])
{
   cls();

   if(argc < 2) {
       //               11111111112222222222333333333344444444445555555555666666
       //      12345678901234567890123456789012345678901234567890123456789001234
   	   printf("MXS FORMAT:\n");
   	   printf("   >MXS (COMMAND)\n\n");
   	   printf("EXAMPLES:\n");
       printf("   >MXS IMAGES            <--- LISTS ALL AVAILABLE IMAGES\n");
   	   printf("   >MXS IMAGES XXX        <--- LISTS IMAGES CONTAINING XXX\n");
   	   printf("   >MXS MOUNT 0 FILE.DSK  <--- MOUNT FILE.DSK TO DRIVE SLOT 0\n");
   	   printf("   >MXS LISTMNT           <--- LIST CURRENTLY MOUNTED IMAGES\n");
   	   return -1;
   }

  strcpy(workBuffer,strupr(trim_space(argv[1])));

  if(strstr(workBuffer,"MOUNT") != NULL) {
     int d = atoi(argv[2]);
     if(d < 0 || d > 3 || !stringIsNumeric(argv[2])) {
         printf("INVALID DRIVE SLOT: (%s), YOU MUST SELECT 0, 1, 2, OR 3.\n\n\n\n", argv[2]);
         return -3;
     }

     mount(d, argv[3]);
  }
  else   
  if(strstr(workBuffer,"IMAGES") != NULL) {
     if(argc == 3)
         images(argv[2]);
     else 
         images("");
  }
  else   
      if(strstr(workBuffer,"LISTMNT") != NULL) {
         listmounts();
  }
  else {
  	printf("INVALID COMMAND: (%s)\nVALID COMMANDS ARE: LISTMNT, IMAGES, MOUNT #\n\n\n\n", argv[1]);
  	return -2;
  }

}
