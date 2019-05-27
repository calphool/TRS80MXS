#include <stdio.h> 
#include <malloc.h>
#include <math.h>

#define byte unsigned char

__sfr __at 0x82 PORTX82;
__sfr __at 0x83 PORTX83;
__sfr __at 0x84 PORTX84;
__sfr __at 0x85 PORTX85;


void cls() {
   void* p;
   p = 0x3c00;
   memset(p, ' ', 0x3ff);
}

void printBits(size_t size, void * ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char bytez;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            bytez = (b[i] >> j) & 1;
            printf("%u", bytez);
        }
    }
    puts("");
}




int main()
{
   int portNum;
   int len;
   byte b1;
   byte b2;
   int i;
   int j;
   int k;
   char sBuffer[99];
   char buf[65];

   cls();

   do {
       printf("256 X TO QUIT\n");
       scanf("%d %s", &portNum, &sBuffer);

       if(portNum != 256) {
       	   len = strlen(sBuffer);
           if(len != 8 && len != 16) {
               printf("%s IS NOT 8 OR 16 CHARS LONG\n", sBuffer);
           }
           else {
               strrev(sBuffer); // flip the string because our data bus is backward
               if(len == 8) {
               	//8
               	   b1 = 0;
                   j = 7;
                   for(i=0;i<8;i++) {
                       if(sBuffer[i] == '1') {
                       	    k = (0x01 << j);
                       	    b1 = b1 + k;
                       }
                       j--;
                   }
                   printf("%d > ", portNum);
                   printBits(sizeof(b1), &b1); printf("\n");
                   printf("\n");
                   switch(portNum) {
                   	   case 0x82:
                   	       PORTX82 = b1;
                   	   break;

                   	   case 0x83:
                   	       PORTX83 = b1;
                   	   break;

                   	   case 0x84:
                   	       PORTX84 = b1;
                   	   break;

                   	   case 0x85:
                   	       PORTX85 = b1;
                   	   break;

                   	   default:
                           printf("INVALID PORT: %d\n", portNum);
                   }
               }
               else {
               	//16
               	   b1 = 0;
               	   b2 = 0;
                   j = 7;
                   for(i=0;i<8;i++) {
                       if(sBuffer[i] == '1') {
                       	    k = (0x01 << j);
                       	    b1 = b1 + k;
                       }
                       j--;
                   }
               	   j = 7;
                   for(i=8;i<16;i++) {
                   	   if(sBuffer[i] == '1') {
                   	   	    k = (0x01 << j);
                   	        b2 = b2 + k;
                   	   }
                   	   j--;
                   }
                   printf("%d > ", portNum);
                   printBits(sizeof(b2), &b2);
                   printf("%d > ", portNum); 
                   printBits(sizeof(b1), &b1);
                   printf("\n");
                   switch(portNum) {
                   	   case 0x82:
                   	       PORTX82 = b2;
                   	       PORTX82 = b1;
                   	   break;

                   	   case 0x83:
                   	       PORTX83 = b2;
                   	       PORTX83 = b1;
                   	   break;

                   	   case 0x84:
                   	       PORTX84 = b2;
                   	       PORTX84 = b1;
                   	   break;

                   	   case 0x85:
                   	       PORTX85 = b2;
                   	       PORTX85 = b1;
                   	   break;

                   	   default:
                           printf("INVALID PORT: %d\n", portNum);
                   }
               }
           }
       }
   }
   while(portNum != 256);
   cls();
}
