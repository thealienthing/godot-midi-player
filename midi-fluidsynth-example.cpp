#include <fluidsynth.h>
#include "MidiFile.h"
#include "Options.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>

using namespace std;
using namespace smf;

fluid_settings_t *settings = NULL;
fluid_synth_t *synth = NULL;
fluid_audio_driver_t *adriver = NULL;

/* Clean up */
void clean_up()
{
   delete_fluid_audio_driver(adriver);
   delete_fluid_synth(synth);
   delete_fluid_settings(settings);
}

int main(int argc, char **argv)
{

   int sfont_id;
   int i, key;

   /* Create the settings. */
   settings = new_fluid_settings();
   if (settings == NULL)
   {
      puts("Failed to create the settings!");
      clean_up();
      return -1;
   }

   /* Change the settings if necessary*/

   /* Create the synthesizer. */
   synth = new_fluid_synth(settings);
   if (synth == NULL)
   {
      puts("Failed to create the synth!");
      clean_up();
      return -1;
   }

   /* Load a SoundFont and reset presets (so that new instruments
    * get used from the SoundFont)
    * Depending on the size of the SoundFont, this will take some time to complete...
    */
   sfont_id = fluid_synth_sfload(synth, "8bitsf.sf2", 1);
   if (sfont_id == FLUID_FAILED)
   {
      puts("Loading the SoundFont failed!");
      clean_up();
      return -1;
   }

   /* Create the audio driver. The synthesizer starts playing as soon
      as the driver is created. */
   adriver = new_fluid_audio_driver(settings, synth);
   if (adriver == NULL)
   {
      puts("Failed to create the audio driver!");
      clean_up();
      return -1;
   }

   Options options;
   options.process(argc, argv);
   MidiFile midifile;
   if (options.getArgCount() == 0)
      midifile.read(cin);
   else
      midifile.read(options.getArg(1));
   midifile.doTimeAnalysis();
   midifile.linkNotePairs();

   int tracks = midifile.getTrackCount();
   int tpq = midifile.getTicksPerQuarterNote();
   int tick_us = 1000000 / tpq;
   cout << "TPQ: " << tpq << endl;
   cout << "uSec per tick " << tick_us << endl;
   if (tracks > 1)
      cout << "TRACKS: " << tracks << endl;

   double lastNoteFinished = 0.0;
   for (int track = 0; track < tracks; track++)
   {
      if (tracks < 1)
      {
         continue;
      }

      for (int event = 0; event < midifile[track].size()-1; event++)
      {
         MidiEvent* mev = &midifile[track][event];
         MidiEvent* mev_next = &midifile[track][event+1];

         if (!mev->isNote()) {
            cout << "Skip" << endl;
            continue;
         }
         
         if (mev->isNoteOn())
         {
            cout << "Note on: " << mev->getKeyNumber() << endl;
            fluid_synth_noteon(synth, 0, mev->getKeyNumber(), 80);
         }
         else if (mev->isNoteOff())
         {
            cout << "Note off: " << mev->getKeyNumber() << endl;
            fluid_synth_noteoff(synth, 0, mev->getKeyNumber());
         }
         
         int silence = static_cast<int>((midifile.getTimeInSeconds(mev_next->tick) - midifile.getTimeInSeconds(mev->tick)) * 1000 * 1000);
         cout << "Silence: " << silence << endl;
         if (silence > 0)
         {
            cout << "Sleep" << endl;
            usleep(silence);
         }

         double duration = mev->getDurationInSeconds();

         MidiEvent *off = mev->getLinkedEvent();
         lastNoteFinished = midifile.getTimeInSeconds(off->tick);
      }
   }

   return 0;
}