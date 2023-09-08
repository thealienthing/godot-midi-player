#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
using namespace smf;

int main(int argc, char** argv) {
   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0) midifile.read(cin);
   else midifile.read(options.getArg(1));
   midifile.doTimeAnalysis();
   midifile.linkNotePairs();

   int tracks = midifile.getTrackCount();
   cout << "TPQ: " << midifile.getTicksPerQuarterNote() << endl;
   if (tracks > 1) cout << "TRACKS: " << tracks << endl;
   for (int track=0; track<tracks; track++) {
      if (tracks > 1) cout << "\nTrack " << track << endl;
      // cout << "Tick\tSeconds\tDur\tMessage" << endl;
      for (int event=0; event<midifile[track].size(); event++) {
         // cout << dec << midifile[track][event].tick;
         // cout << '\t' << dec << midifile[track][event].seconds;
         // cout << '\t';
         auto e = midifile[track][event];
         if (e.isNoteOn()){
            cout << "Key up: " << e.getKeyNumber() << " | Duration: " << midifile[track][event].getDurationInSeconds();
            // usleep(1000000000000*(e.getDurationInSeconds()));
            usleep(1000000.0*midifile[track][event].getDurationInSeconds());
         }
         // cout << '\t' << hex;
         // for (int i=0; i<midifile[track][event].size(); i++)
            // cout << (int)midifile[track][event][i] << ' ';
         cout << endl;
      }
   }


   return 0;
}