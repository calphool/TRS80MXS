
#include <stdio.h> 
#include <malloc.h>

#define byte unsigned char
#define mallocblock 1024
#define HOLD_TIME 1

__sfr __at 0x82 PORTX82;
__sfr __at 0x83 PORTX83;
__sfr __at 0x84 PORTX84;
__sfr __at 0x85 PORTX85;


long heap;
uint memBlockPtr = 47000;

/*
typedef struct  {
    int pitch;
    byte volume;
}
Voice;
*/

int vArray_pitch[12];
byte vArray_volume[12];

char buf[4];
uint temp2;
uint temp;

struct Node
{
	uint row;
    byte col;
    uint pitch;
    byte volume;
    struct Node* next; 
};

uint nodeSize;

/* function to insert a new_node in a list. Note that this 
  function expects a pointer to head_ref as this can modify the 
  head of the input linked list (similar to push())*/
void sortedInsert(struct Node** head_ref, struct Node* new_node) 
{ 
    struct Node* current; 
    /* Special case for the head end */
    if (*head_ref == NULL || (*head_ref)->row >= new_node->row) 
    { 
        new_node->next = *head_ref; 
        *head_ref = new_node; 
    } 
    else
    { 
        /* Locate the node before the point of insertion */
        current = *head_ref; 
        while (current->next!=NULL && current->next->row < new_node->row) { 
            current = current->next; 
        } 

        while(current->next!=NULL && current->next->row == new_node->row && current->next->col < new_node->col) {
            current = current->next;
        }
        new_node->next = current->next; 
        current->next = new_node; 
    } 
} 


struct Node *newNode(uint row, byte col, int pitch, byte volume) 
{ 
    uint total;
    uint largest;

    mallinfo(&total, &largest);

    if(total <= nodeSize) {
        sbrk(memBlockPtr,mallocblock);
        memBlockPtr = memBlockPtr - mallocblock;
    }

    struct Node* new_node = (struct Node*) malloc(nodeSize); 
    if(new_node == NULL) {
    	printf("unable to allocate node\n");
    	return NULL;
    }
  
    new_node->row  = row;
    new_node->col = col;
    new_node->pitch = pitch;
    new_node->volume = volume;
    new_node->next =  NULL; 
  
    return new_node; 
} 

/*
void printList(struct Node *head) 
{ 
    struct Node *temp = head; 
    while(temp != NULL) 
    { 
        printf("(%d,%d)=%d/%d,", temp->row, temp->col, temp->pitch, temp->volume); 
        temp = temp->next; 
    } 
    printf("\n");
} 
*/

void hold(unsigned int x) {
    for(unsigned int y = 0; y<x;y++) {
      #pragma asm
      nop
      #pragma endasm
    }
}


unsigned char bitArray[256] = {0,128,64,192,32,160,96,224,16,144,80,208,48,176,112,240,8,136,72,200,40,168,104,232,24,152,
    88,216,56,184,120,248,4,132,68,196,36,164,100,228,20,148,84,212,52,180,116,244,12,140,76,204,44,172,108,236,28,156,92,
    220,60,188,124,252,2,130,66,194,34,162,98,226,18,146,82,210,50,178,114,242,10,138,74,202,42,170,106,234,26,154,90,218,
    58,186,122,250,6,134,70,198,38,166,102,230,22,150,86,214,54,182,118,246,14,142,78,206,46,174,110,238,30,158,94,222,62,
    190,126,254,1,129,65,193,33,161,97,225,17,145,81,209,49,177,113,241,9,137,73,201,41,169,105,233,25,153,89,217,57,185,
    121,249,5,133,69,197,37,165,101,229,21,149,85,213,53,181,117,245,13,141,77,205,45,173,109,237,29,157,93,221,61,189,125,
    253,3,131,67,195,35,163,99,227,19,147,83,211,51,179,115,243,11,139,75,203,43,171,107,235,27,155,91,219,59,187,123,251,7,
    135,71,199,39,167,103,231,23,151,87,215,55,183,119,247,15,143,79,207,47,175,111,239,31,159,95,223,63,191,127,255}; 

/*
unsigned char reverseBits(unsigned char v)
{
//    v = ((v >> 1) & 0x55) | ((v & 0x55) << 1);
//    v = ((v >> 2) & 0x33) | ((v & 0x33) << 2);
//    v = ((v >> 4) & 0x0F) | ((v & 0x0F) << 4);
//    return v;
    return bitArray[v];
}
*/

#define reverseBits(v) bitArray[v]


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
}

byte bPastVolume[12] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

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
    
    if(bPastVolume[bVoiceNumber] != volume) {
    	setVolume(bPort, bChipVoiceNum, 0x0f - volume);
    	bPastVolume[bVoiceNumber] = volume;
    }

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
}


void cls() {
   void* p;

   p = 0x3c00;
   
   memset(p, ' ', 0x3ff);
}


#define charat(x,y,c)  memset(0x3c00+((y<<6) + x), c, 1);


void h_lineat(byte y, byte c) {
    void* p;
    uint cc = y;
    cc = cc * 64;

    p = 0x3c00 + cc;

    memset(p, c, 64);
}


void v_lineat(byte x, byte c) {
    for(uint j = 0; j < 16; j++) 
        charat(x, j, c);
}


void strat(byte x, byte y, char* s) {
    temp2 = strlen(s);
    for(uint i = 0; i < temp2; i++) {
        charat(x+i,y,*(s+i));
    }
}
 

void drawGrid() {
   uint j;
   char buf[7];

   j=1;
   for(uint i = 2;i<64;i=i+5) {
     v_lineat(i+1, 0x95);
     itoa(j, buf, 10);
     if(j < 13)
       strat(i+2,0,buf);
     j++;
   }

   h_lineat(12, 0x8c);
   h_lineat(13, ' ');
   h_lineat(14, ' ');
   h_lineat(15, ' ');
   strat(0,15,"YYYY-MM-DD-HH-MM-SS");
}


char mult_lu1[12] = {4, 9, 14, 19, 24, 29, 34, 39, 44, 49, 54, 59};
char mult_lu2[12] = {7,12, 17, 22, 27, 32, 37, 42, 47, 52, 57, 62};

void playVoices() { 
    register byte i;
    // scroll screen   
    memcpy(15424,15488,64);
    memcpy(15488,15552,64);
    memcpy(15552,15616,64);
    memcpy(15616,15680,64);
    memcpy(15680,15744,64);
    memcpy(15744,15808,64);
    memcpy(15808,15872,64);
    memcpy(15872,15936,64);
    memcpy(15936,16000,64);
    memcpy(16000,16064,64);

    for(i=0;i<12;i++) {
        sound(i, vArray_pitch[i], vArray_volume[i]);
        itoa(vArray_pitch[i], buf, 16);
        temp = vArray_volume[i];
        if(temp < 0x0a)
           temp+=48;
        else
           temp+=55;
        strat(mult_lu1[i], 11, buf);
        charat(mult_lu2[i],11, temp);
    }
}



void silence() {
    for(uint i=255;i>=249;i=i-2) {
        PORTX82 = i;
        PORTX83 = i;
        PORTX84 = i;
        PORTX85 = i;
    }
}

void play(struct Node *head) {
    byte prevRow = 999;

    struct Node *temp = head;
    prevRow = head->row;

    silence();

    do { 
        temp = temp->next;
        if(temp == NULL) {
            playVoices();
            hold(HOLD_TIME);
        }
        else {
            vArray_pitch[temp->col] = temp->pitch;
            vArray_volume[temp->col] = temp->volume;

            if(temp->row == prevRow) {
            }
            else {
                playVoices();
                hold(HOLD_TIME * ((temp->row) - prevRow));
                prevRow = temp->row;
            }
        }
    } while(temp != NULL);

    silence();
} 

uint throbctr = 0;


void throb() {
    throbctr++;
    switch((uint)(throbctr % 6)) {
        case 0:
            charat(63,15,129); // top left
            break;
        case 1:
            charat(63,15,130); // top right
            break;
        case 2:
            charat(63,15,136); // mid right
            break;
        case 3:
            charat(63,15,160); // bot right
            break;
        case 4:
            charat(63,15,144); // bot left
            break;
        default:
            charat(63,15,132); // mid left
    }
}


struct Node* head;

void loadData() { 
   head = NULL;

   for(uint i=0;i<256;i++) {
    throb();
    sortedInsert(&head, newNode(rand()%1024,i%12,rand()%1000,15));
   }
   charat(63,15,' ');    
}


int main()
{
   nodeSize = sizeof(struct Node);
   mallinit();

   memset(vArray_pitch, 0, sizeof(vArray_pitch));
   memset(vArray_volume, 0, sizeof(vArray_volume));

   cls();
   drawGrid();
   loadData();

   play(head);

   strat(30,15,"Done.");
   silence();
   getchar();
}