#include <stdio.h> 
#include <malloc.h>


/*

Freq    Ratio   Data    Rounded Data        Voices                                          
                                        0   1   2   3   4   5   6   7   8   9   10  11

                    1023       1023 a   2   2   2   2   2   2   2   0   0   0   0   0
1975.53 0.943878352 965.5875537 966 a#  2   2   2   2   2   2   2   0   0   0   0   0
1864.66 0.943871805 911.3908673 911 b   2   2   2   2   2   2   2   0   0   0   0   0
1760    0.943875    860.2390548 860 c   2   2   2   2   2   2   2   0   0   0   0   0
1661.22 0.943872576 811.9560523 812 c#  2   2   2   2   2   2   2   0   0   0   0   0
1567.98 0.943876835 766.3865089 766 d   2   2   2   2   2   2   2   0   0   0   0   0
1479.98 0.943870863 723.3698957 723 d#  2   2   2   2   2   2   2   0   0   0   0   0
1396.91 0.943876127 682.7715752 683 e   2   2   2   2   2   2   2   0   0   0   0   0
1318.51 0.943876042 644.4517319 644 f   2   2   2   2   2   2   2   0   0   0   0   0
1244.51 0.943873492 608.2809069 608 f#  2   2   2   2   2   2   2   0   0   0   0   0
1174.66 0.943873121 574.1399979 574 g   2   2   2   2   2   2   2   0   0   0   0   0
1108.73 0.943872719 541.915081  542 g#  2   2   2   2   2   2   2   0   0   0   0   0
1046.5  0.943879599 511.5025892 512 a   3   3   3   3   3   3   3   1   1   1   1   1
987.77  0.943873574 482.7937769 483 a#  3   3   3   3   3   3   3   1   1   1   1   1
932.33  0.943871805 455.6954336 456 b   3   3   3   3   3   3   3   1   1   1   1   1
880     0.943875    430.1195274 430 c   3   3   3   3   3   3   3   1   1   1   1   1
830.61  0.943872576 405.9780261 406 c#  3   3   3   3   3   3   3   1   1   1   1   1
783.99  0.943876835 383.1932545 383 d   3   3   3   3   3   3   3   1   1   1   1   1
739.99  0.94387762  361.687537  362 d#  3   3   3   3   3   3   3   1   1   1   1   1
698.46  0.943862211 341.3831984 341 e   3   3   3   3   3   3   3   1   1   1   1   1
659.25  0.943875616 322.2232768 322 f   3   3   3   3   3   3   3   1   1   1   1   1
622.25  0.943881077 304.1404534 304 f#  3   3   3   3   3   3   3   1   1   1   1   1
587.33  0.943881634 287.0725881 287 g   3   3   3   3   3   3   3   1   1   1   1   1
554.37  0.943864206 270.9575405 271 g#  3   3   3   3   3   3   3   1   1   1   1   1
523.25  0.943870043 255.7487054 256 a   4   4   4   4   4   4   4   2   2   2   2   2
493.88  0.943873006 241.3942993 241 a#  4   4   4   4   4   4   4   2   2   2   2   2
466.16  0.943881929 227.8477168 228 b   4   4   4   4   4   4   4   2   2   2   2   2
440     0.943863636 215.0571745 215 c   4   4   4   4   4   4   4   2   2   2   2   2
415.3   0.943895979 202.9916023 203 c#  4   4   4   4   4   4   4   2   2   2   2   2
392     0.943852041 191.5940381 192 d   4   4   4   4   4   4   4   2   2   2   2   2
369.99  0.943890375 180.8437685 181 d#  4   4   4   4   4   4   4   2   2   2   2   2
349.23  0.943876528 170.6941884 171 e   4   4   4   4   4   4   4   2   2   2   2   2
329.63  0.943876468 161.1142276 161 f   4   4   4   4   4   4   4   2   2   2   2   2
311.13  0.943849838 152.0676375 152 f#  4   4   4   4   4   4   4   2   2   2   2   2
293.66  0.943880678 143.5337049 144 g   4   4   4   4   4   4   4   2   2   2   2   2
277.18  0.943899271 135.4813594 135 g#  4   4   4   4   4   4   4   2   2   2   2   2
261.63  0.943852005 127.8743527 128 a   5   5   5   5   5   5   5   3   3   3   3   3
246.94  0.943873006 120.6971496 121 a#  5   5   5   5   5   5   5   3   3   3   3   3
233.08  0.943881929 113.9238584 114 b   5   5   5   5   5   5   5   3   3   3   3   3
220     0.943863636 107.5285873 108 c   5   5   5   5   5   5   5   3   3   3   3   3
207.65  0.943895979 101.4958011 101 c#  5   5   5   5   5   5   5   3   3   3   3   3
196     0.943877551 95.79960821 96  d   5   5   5   5   5   5   5   3   3   3   3   3
185     0.943837838 90.41929508 90  d#  5   5   5   5   5   5   5   3   3   3   3   3
174.61  0.943874921 85.34450502 85  e   5   5   5   5   5   5   5   3   3   3   3   3
164.81  0.943874765 80.55452461 81  f   5   5   5   5   5   5   5   3   3   3   3   3
155.56  0.943880175 76.03381877 76  f#  5   5   5   5   5   5   5   3   3   3   3   3
146.83  0.943880678 71.76685244 72  g   5   5   5   5   5   5   5   3   3   3   3   3
138.59  0.943863194 67.73809054 68  g#  5   5   5   5   5   5   5   3   3   3   3   3
130.81  0.943888082 63.93717635 64  a   6   6   6   6   6   6   6   4   4   4   4   4
123.47  0.943873006 60.34857481 60  a#  6   6   6   6   6   6   6   4   4   4   4   4
116.54  0.943881929 56.9619292  57  b   6   6   6   6   6   6   6   4   4   4   4   4
110     0.943909091 53.76688281 54  c   6   6   6   6   6   6   6   4   4   4   4   4
103.83  0.943850525 50.74790056 51  c#  6   6   6   6   6   6   6   4   4   4   4   4
98      0.943877551 47.8998041  48  d   6   6   6   6   6   6   6   4   4   4   4   4
92.5    0.943891892 45.21223672 45  d#  6   6   6   6   6   6   6   4   4   4   4   4
87.31   0.943878135 42.67484169 43  e   6   6   6   6   6   6   6   4   4   4   4   4
82.41   0.943817498 40.2772623  40  f   6   6   6   6   6   6   6   4   4   4   4   4
77.78   0.943944459 38.01949856 38  f#  6   6   6   6   6   6   6   4   4   4   4   4
73.42   0.9438845   35.8860154  36  g   6   6   6   6   6   6   6   4   4   4   4   4
69.3    0.943867244 33.87163445 34  g#  6   6   6   6   6   6   6   4   4   4   4   4
65.41   0.943892371 31.97117735 32  a   7   7   7   7   7   7   7   5   5   5   5   5
61.74   0.943796566 30.17428741 30  a#  7   7   7   7   7   7   7   5   5   5   5   5
58.27   0.943881929 28.4809646  28  b   7   7   7   7   7   7   7   5   5   5   5   5
55      0.943818182 26.88085223 27  c   7   7   7   7   7   7   7   5   5   5   5   5
51.91   0.943941437 25.37395028 25  c#  7   7   7   7   7   7   7   5   5   5   5   5
49      0.943877551 23.94990205 24  d   7   7   7   7   7   7   7   5   5   5   5   5
46.25   0.943783784 22.60352918 23  d#  7   7   7   7   7   7   7   5   5   5   5   5
43.65   0.943871707 21.33483167 21  e   7   7   7   7   7   7   7   5   5   5   5   5
41.2    0.943932039 20.13863115 20  f   7   7   7   7   7   7   7   5   5   5   5   5
38.89   0.943944459 19.00974928 19  f#  7   7   7   7   7   7   7   5   5   5   5   5
36.71   0.9438845   17.9430077  18  g   7   7   7   7   7   7   7   5   5   5   5   5
34.65   0.943722944 16.93322805 17  g#  7   7   7   7   7   7   7   5   5   5   5   5
32.7    0.944036697 15.98558868 16  a   8   8   8   8   8   8   8   6   6   6   6   6
30.87   0.943958536 15.08973288 15  a#  8   8   8   8   8   8   8   6   6   6   6   6
29.14   0.943719973 14.2404823  14  b   8   8   8   8   8   8   8   6   6   6   6   6
27.5    0.944       13.44301529 13  c   8   8   8   8   8   8   8   6   6   6   6   6
25.96   0.94375963  12.68697514 13  c#  8   8   8   8   8   8   8   6   6   6   6   6
24.5    0.943673469 11.97236185 12  d   8   8   8   8   8   8   8   6   6   6   6   6
23.12   0.944204152 11.30435377 11  d#  8   8   8   8   8   8   8   6   6   6   6   6
21.83   0.94365552  10.66741583 11  e   8   8   8   8   8   8   8   6   6   6   6   6
20.6    0.944174757 10.07190475 10  f   8   8   8   8   8   8   8   6   6   6   6   6
19.45   0.94344473  9.502285463 10  f#  8   8   8   8   8   8   8   6   6   6   6   6
18.35   0.94386921  8.968914671 9   g   8   8   8   8   8   8   8   6   6   6   6   6
17.32   0.943995381 8.466614023 8   g#  8   8   8   8   8   8   8   6   6   6   6   6

*/




#define byte unsigned char

__sfr __at 0x82 PORTX82;
__sfr __at 0x83 PORTX83;
__sfr __at 0x84 PORTX84;
__sfr __at 0x85 PORTX85;


char aBuffer[128];

unsigned char bitArray[256] = {0,128,64,192,32,160,96,224,16,144,80,208,48,176,112,240,8,136,72,200,40,168,104,232,24,152,
    88,216,56,184,120,248,4,132,68,196,36,164,100,228,20,148,84,212,52,180,116,244,12,140,76,204,44,172,108,236,28,156,92,
    220,60,188,124,252,2,130,66,194,34,162,98,226,18,146,82,210,50,178,114,242,10,138,74,202,42,170,106,234,26,154,90,218,
    58,186,122,250,6,134,70,198,38,166,102,230,22,150,86,214,54,182,118,246,14,142,78,206,46,174,110,238,30,158,94,222,62,
    190,126,254,1,129,65,193,33,161,97,225,17,145,81,209,49,177,113,241,9,137,73,201,41,169,105,233,25,153,89,217,57,185,
    121,249,5,133,69,197,37,165,101,229,21,149,85,213,53,181,117,245,13,141,77,205,45,173,109,237,29,157,93,221,61,189,125,
    253,3,131,67,195,35,163,99,227,19,147,83,211,51,179,115,243,11,139,75,203,43,171,107,235,27,155,91,219,59,187,123,251,7,
    135,71,199,39,167,103,231,23,151,87,215,55,183,119,247,15,143,79,207,47,175,111,239,31,159,95,223,63,191,127,255}; 

#define reverseBits(v) bitArray[v]

void printBits(char* sLabel, size_t size, void * ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char bytez;
    int i, j;

    printf("%s ", sLabel);

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            bytez = (b[i] >> j) & 1;
            printf("%u", bytez);
        }
    }
    printf(" ");
}


void printKey(byte b) {
    byte temp;
    static int reg = 0;
    int temp2;

    if(b >= 128) {
        printf("= LTCH,");
        b = b & 0x7f;
        temp = b & 0x60;
        temp = temp >> 5;
        printf("CH=%d,",temp);
        temp = b;
        temp = temp & 0x10;
        if(temp > 0) {
            printf("VOL, ");
            reg = 0;
        }
        else
            printf("NOTE,");
        temp = b;
        temp = temp & 0x0f;
        reg = reg & 0xfff0;
        reg = reg | temp;
        printf(" REG=%04X\n",reg);
    }
    else {
        temp = b;
        temp = temp & 0x3f;
        reg = reg & 0x000f;
        temp2 = temp;
        temp2 = temp2 << 4;
        reg = reg | temp2;
        printf("= DATA,           REG=%04X\n",reg);
    }
}

void setVolume(byte bPort, byte bChipVoiceNum, byte bVolume) {
	byte bx;

	bx = (bChipVoiceNum << 5);
	bx = (bx | 0x90);
    bx = (bx | bVolume);
    bx = reverseBits(bx);

    switch(bPort) {
    	case 0x82:
    	    PORTX82 = bx;
    	break;

    	case 0x83:
    	    PORTX83 = bx;
    	break;

    	case 0x84:
    	    PORTX84 = bx;
    	break;

    	default:
    	    PORTX85 = bx;
    }   

    bx = reverseBits(bx);
    sprintf(aBuffer, "%d >", bPort);
    printBits(aBuffer, sizeof(bx), &bx);
    printKey(bx);
}

//byte bPastVolume[12] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

void sound(byte bVoiceNumber, int pitch, byte volume) {
    byte bPort;
    byte bChipVoiceNum;
    byte rightfourbits;
    byte leftsixbits;
    byte b1;

    if(bVoiceNumber < 3) {
    	bPort = 0x82;
    	bChipVoiceNum = bVoiceNumber;
    }
    else if(bVoiceNumber < 6) {
    	bPort = 0x83;
        bChipVoiceNum = bVoiceNumber - 3;
    }
    else if(bVoiceNumber < 9) {
    	bPort = 0x84;
    	bChipVoiceNum = bVoiceNumber - 6;
    }
    else {
    	bPort = 0x85;
    	bChipVoiceNum = bVoiceNumber - 9;
    }
    
//    if(bPastVolume[bVoiceNumber] != volume) {
    	setVolume(bPort, bChipVoiceNum, 0x0f - volume);
//    	bPastVolume[bVoiceNumber] = volume;
//    }

    rightfourbits = (pitch & 0x000f);
    leftsixbits =   (byte)((int)(pitch & 0x03f0) >> 4);
    rightfourbits = (rightfourbits | 0x80);
    bChipVoiceNum = reverseBits(bChipVoiceNum) >> 5;
    b1 = reverseBits(rightfourbits);
    b1 = (b1 | bChipVoiceNum);
    leftsixbits = reverseBits(leftsixbits);

    switch(bPort) {
    	case 0x82:
    	    PORTX82 = b1;
    	    PORTX82 = leftsixbits;
    	break;

    	case 0x83:
    	    PORTX83 = b1;
    	    PORTX83 = leftsixbits;
    	break;

    	case 0x84:
    	    PORTX84 = b1;
    	    PORTX84 = leftsixbits;
    	break;

    	default:
    	    PORTX85 = b1;
    	    PORTX85 = leftsixbits;
    }

    sprintf(aBuffer, "%d >", bPort);
    b1 = reverseBits(b1);
    printBits(aBuffer, sizeof(b1), &b1);
    printKey(b1);
    leftsixbits = reverseBits(leftsixbits);
    printBits(aBuffer, sizeof(leftsixbits), &leftsixbits);
    printKey(leftsixbits);
}


void silence() {
    for(uint i=255;i>=249;i=i-2) {
        PORTX82 = i;
        PORTX83 = i;
        PORTX84 = i;
        PORTX85 = i;
    }
}


void cls() {
   void* p;
   p = 0x3c00;
   memset(p, ' ', 0x3ff);
}



int main()
{
   cls();

   int voice;
   int pitch;
   int volume;

   silence();

   do {
       printf("(VOICE) (PITCH) (VOLUME), 255 0 0 TO QUIT\n");
       scanf("%d %d %d", &voice, &pitch, &volume);
       if(voice == 255 || (voice < 12 && volume < 16)) {
           if(voice == 255) 
               printf("SHUTTING DOWN.\n");
           else
               sound((byte) voice, pitch, (byte) volume);
       }
       else 
           printf("INVALID VOICE OR VOLUME.\n");
   }
   while(voice != 255);

   silence();
   printf("\n\nDONE.\n\n");
}
