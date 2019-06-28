#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
 
char workBuffer[255];
char tempBuffer[64];


char * trim_space(char *str) {
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


void sendCommand(char* s) {
   char c;
   for(int i=0;i<strlen(s);i++) {
   	  c = *(s+i);
   	  memset(0x37e8, c, 1);
   }
   memset(0x37e8,0x0a,1);
}


char getByteFromTeensy() {
	char* c = 0x37e8;
	return (*c);
}


int getRespString() {
	workBuffer[0] = 0x0;
    int ctr = 0;
    char c;

    do {
        c = getByteFromTeensy();
        if(c != 0x0a && c != 0x0d) {
        	workBuffer[ctr] = c;
        	workBuffer[ctr+1] = 0x0;
        	ctr++;
        	if(ctr > 254)
        		return -1;
        }
    }
    while(c != 0x0a && c != 0x0d);

    return strlen(workBuffer);
}


int readTeensyRespUntilCR() {
	int ctr = 0;
    char c;
	do {
       c = getByteFromTeensy();
       ctr++;
       if(ctr > 128)    return -1;
	}
	while(c != 0x0a && c != 0x0d);

	return 0;
}


void mount(int iDriveNum, char* driveImageName) {
	if(readTeensyRespUntilCR() == -1) {
		printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
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
    printf("%s\n", strupr(workBuffer));
}


void images(char* sPrefix) {
  char buff[128];

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
    getRespString();

    int i = atoi(workBuffer);
    for(int x = 0; x < i; x++) {
       sprintf(tempBuffer,"IMGAT %d", x);
       sendCommand(tempBuffer);
	   if(readTeensyRespUntilCR() == -1) {
			printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
			return;
	   }
       getRespString();
       strcpy(workBuffer, strupr(workBuffer));
       if(strlen(buff) == 0 || (strstr(workBuffer,buff) != NULL))
           printf("%s\n", workBuffer);	   
    }

    printf("\n\n\n");
}

void listmounts() {
  int x = 0;

	strcpy(tempBuffer, "LISTMOUNTS");
	sendCommand(tempBuffer);

    if(readTeensyRespUntilCR() == -1) {
		printf("  INVALID RESPONSE FROM EMULATED DRIVE.\n");
		return;
    }

    printf("\n\n\n");
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

void cls() {
   void* p = 0x3c00;
   memset(p, ' ', 0x3ff);
}


int main(int argc, char *argv[])
{
   cls();

   if(argc < 2) {
       printf("\n\n");
   	   printf("TRS80MXS FORMAT:\n");
   	   printf("  TRS80MXS (COMMAND)\n\n");
   	   printf("EXAMPLES:\n");
       printf("  TRS80MXS IMAGES\n");
   	   printf("  TRS80MXS IMAGES (PREFIX LETTER)\n");
   	   printf("  TRS80MXS MOUNT 1 MYFILE.DSK\n");
   	   printf("  TRS80MXS LISTMNT\n\n\n");
   	   return -1;
   }

  strcpy(workBuffer,strupr(trim_space(argv[1])));

  if(strstr(workBuffer,"MOUNT") != NULL) {
     mount(atoi(argv[2]), argv[3]);
  }
  else   
  if(strstr(workBuffer,"IMAGES") != NULL) {
     if(argc == 3) {
         images(argv[2]);
     }
     else {
         images("");
     }
  }
  else   
  if(strstr(workBuffer,"LISTMNT") != NULL) {
     listmounts();
  }
  else {
  	printf("INVALID COMMAND: (%s)\n", argv[1]);
  	return -2;
  }
}
