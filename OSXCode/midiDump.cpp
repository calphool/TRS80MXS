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


MidiEventList mOutTracks[12];
char sFileName[20];

int couldFit(MidiEvent* m, int j) {
  int iStartTick = m->tick;
  int iEndTick = (m->getLinkedEvent())->tick;
  mOutTracks[j].linkNotePairs();

  for(int i=0;i < mOutTracks[j].getSize();i++) {
      if(mOutTracks[j][i].isNoteOn() && mOutTracks[j][i].isLinked()) {
        MidiEvent* me = mOutTracks[j][i].getLinkedEvent();
        int iThisNoteStartTick = mOutTracks[j][i].tick;
        int iThisNoteEndTick = me->tick;

        if(iThisNoteEndTick >= iStartTick && iThisNoteEndTick <= iEndTick)
           return 0;

        if(iThisNoteStartTick >= iStartTick && iThisNoteStartTick <= iEndTick)
           return 0;

        if(iThisNoteStartTick <= iStartTick && iThisNoteEndTick >= iEndTick)
           return 0;

        if(iThisNoteStartTick >= iStartTick && iThisNoteEndTick <= iEndTick)
           return 0;
      }
  }

  return 1;
}

void sprayNote(MidiEvent* m) {
     int j=0;
     char bFoundTrack = 0;
     vector<uchar> midiOutEvent;
     midiOutEvent.resize(3);

     do {
         if(couldFit(m,j) == 1) {
             mOutTracks[j].append(*m);
             mOutTracks[j].append(*m->getLinkedEvent());
             bFoundTrack = 1;
         }
         j++;
     } while(bFoundTrack == 0 && j < 12);

     if(j == 12) 
         printf("Warning:  note dropped %d %d %d.\n", (*m)[0], (*m)[1], (*m)[2]);
}


std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}


int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return numToRound + multiple - remainder;
}


void dumpMidiFile(char* s) {
   MidiFile midifile;
   
   midifile.read(s);
   midifile.doTimeAnalysis();
   midifile.linkNotePairs();
   int tracks = midifile.getTrackCount();
   if (tracks < 1) {
      printf("No tracks found.\n");
      exit(-1);
   } 

   printf("//%s\n", s);
   printf("//Tracks: %d\n", tracks);

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


   printf("//Shortest note duration: %f\n\n", dShortestDuration);
   int iMultiple = (int)((dShortestDuration * 1000));
   iMultiple = roundUp(iMultiple,25);
   printf("int mscounter = %d;\n", 10 * iMultiple / 25);

   printf("typedef struct {\n  int iOffset;\n  char iChannel;\n  int iPitch;\n  char iVolume;\n} notestruct;\n\n");

   printf("const notestruct ns[] = {\n");
   int pa;
   int iArrayLen = 0;
   std::vector<string> sBuff;
   char buff[30];
   for (int track=0; track<tracks; track++) {
      for (int event=0; event < midifile[track].size(); event++) {
           if(midifile[track][event].isNoteOn()) {
               if(midifile[track][event][1] >= 12) pa = pitchArray[midifile[track][event][1] - 12];
               if(midifile[track][event][1] >= 12) pa = pitchArray[midifile[track][event][1] - 12];
               if(midifile[track][event][1] >= 12) pa = pitchArray[midifile[track][event][1] - 12];

               sprintf(buff,"{%06d,%d,%d,%d}", (int)((midifile[track][event].seconds / dShortestDuration) + 0.5),track, pa, midifile[track][event][2]/8);
               sBuff.push_back(buff);
               iArrayLen++;
           }
           else if(midifile[track][event].isNoteOff())  {
               char bNoteOnAtThisTime = 0;
               for(int j=0;j < midifile[track].size();j++) {
                  if(midifile[track][j].isNoteOn() && midifile[track][j].seconds == midifile[track][event].seconds) {
                     // there's already a note for this, don't emit a NOTE OFF
                    bNoteOnAtThisTime = 1;
                  }
               }
               if(bNoteOnAtThisTime == 0) {
                   if(midifile[track][event][1] >= 12) pa = pitchArray[midifile[track][event][1] - 12];
                   if(midifile[track][event][1] >= 12) pa = pitchArray[midifile[track][event][1] - 12];
                   if(midifile[track][event][1] >= 12) pa = pitchArray[midifile[track][event][1] - 12];

                   sprintf(buff,"{%06d,%d,%d,0}", (int)((midifile[track][event].seconds / dShortestDuration) + 0.5),track, pa);
                   sBuff.push_back(buff);
                   iArrayLen++;
                }
           }
      }
   }

   sort(sBuff.begin(), sBuff.end());


   int v = 0;
   for (vector<string>::iterator t=sBuff.begin(); t!=sBuff.end(); ++t) {
        v++;
        if(t == sBuff.end()-1) {
            std::string s = ReplaceAll(std::string(t->c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            printf("%s", s.c_str());
        }
        else { 
            std::string s = ReplaceAll(std::string(t->c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            s = ReplaceAll(std::string(s.c_str()), std::string("{0"), std::string("{"));
            printf("%s,", s.c_str());
        }

        if(v % 8 == 0)
            printf("\n");
   }

   printf("};\n");
   printf("int nsArrayLen = %d;\n", iArrayLen);
}

char* splitMidiFile(char* sIn) {
   vector<uchar> midiOutEvent;
   MidiFile inputFile;

   setbuf(stdout, NULL);

   for(int i=0;i<12;i++) {
       mOutTracks[i] = *(new MidiEventList());
   }

   inputFile.read(sIn);
   inputFile.doTimeAnalysis();
   inputFile.linkNotePairs();

   int inputTracks = inputFile.getTrackCount();

   midiOutEvent.resize(3);
   for (int track=0; track<inputTracks; track++) {
      for (int event=0; event < inputFile[track].size(); event++) {
          if(inputFile[track][event].isNoteOn() && inputFile[track][event].isLinked()) {
            sprayNote(&(inputFile[track][event]));
          }
      }
   }

   MidiFile outputFile;      
   outputFile.absoluteTicks();
   outputFile.addTrack(11);
   outputFile.setTicksPerQuarterNote(inputFile.getTicksPerQuarterNote());

   for(int i=0;i < 12; i++) {
     for(int j=0;j < mOutTracks[i].getSize();j++) {
        outputFile.addEvent(i, mOutTracks[i][j]);
     }
   }

   outputFile.sortTracks();
   strcpy(sFileName,sIn);
   sFileName[strlen(sFileName)-4] = 0x0;
   strcat(sFileName,"_split.mid");
   outputFile.write(sFileName);

   return sFileName;
}



int main(int argc, char** argv) {
   dumpMidiFile(splitMidiFile(argv[1]));
}