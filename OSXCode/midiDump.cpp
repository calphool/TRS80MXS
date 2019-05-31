#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace smf;


int pitchArray[128] = {
860,812,766,723,683,644,608,574,542,1023,966,911,860,812,766,723,683,644,608,574,542,1023,966,911,860,812,766,723,683,644,608,574,
542,512,483,456,430,406,383,362,341,322,304,287,271,256,241,228,215,203,192,181,171,161,152,144,135,128,121,114,108,102,96,90,85,81,
76,72,68,64,60,57,54,51,48,45,43,40,38,36,34,32,30,28,27,25,24,23,21,20,19,18,17,16,15,14,27,25,24,23,21,20,19,18,17,16,15,14,27,25,
24,23,21,20,19,18,17,16,15,14,27,25,24,23,21,20,19,18
};

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0) midifile.read(cin);
   else midifile.read(options.getArg(1));
   midifile.doTimeAnalysis();
   midifile.linkNotePairs();


   int tracks = midifile.getTrackCount();
   if (tracks < 1) {
   	  printf("No tracks found.\n");
   	  return -1;
   } 

   printf("Tracks: %d\n", tracks);

   double dShortestDuration = 99999;
   for (int track=0; track<tracks; track++) {
      for (int event=0; event < midifile[track].size(); event++) {
      	if(midifile[track][event].isNoteOn() && midifile[track][event].isLinked()) {
             MidiEvent* me = midifile[track][event].getLinkedEvent();
             if(me->isNoteOff()) {
             	double dif = me->seconds - midifile[track][event].seconds;
             	if(dShortestDuration > dif)
             		dShortestDuration = dif;
             }
      	} 
      }
   }


   printf("Shortest note duration: %f\n", dShortestDuration);

   printf("const char* arr[] = {");
   for (int track=0; track<tracks; track++) {
      for (int event=0; event < midifile[track].size(); event++) {
      	   if(midifile[track][event].isNoteOn()) {
               printf("\"%03x,%01x,%03x,%01x\",\n", (int)(midifile[track][event].seconds / dShortestDuration),midifile[track][event].getChannelNibble(), pitchArray[midifile[track][event][1] - 12], midifile[track][event][2]/8);
           }
           else if(midifile[track][event].isNoteOff())  {
               char bNoteOnAtThisTime = 0;
               for(int j=0;j < midifile[track].size();j++) {
               	  if(midifile[track][j].isNoteOn() && midifile[track][j].seconds == midifile[track][event].seconds) {
               	     // there's already a note for this, don't emit a NOTE OFF
               	  	bNoteOnAtThisTime = 1;
               	  }
               }
               if(bNoteOnAtThisTime == 0)
               	    printf("\"%03x,%01x,%03x,0\",\n", (int)(midifile[track][event].seconds / dShortestDuration), midifile[track][event].getChannelNibble(), pitchArray[midifile[track][event][1] - 12]);
           }
      }
      printf("\n");
   }
   printf("}\n");
}