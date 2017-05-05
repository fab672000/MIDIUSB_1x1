#pragma once
// MidiUtils Class declaration
//
// Author: Fabien
// License: GPL V3.0 © 2016 Fabien (https://github.com/fab672000)

#include <Arduino.h>
#include <Midi_Defs.h>
#include "Leds.h"
#include "MidiBridge.h"

namespace midi
{
  class MidiUtils
  {
    public:
      //! Send to the serial midi out port AllSoundOff and ResetAllControllers midi cc message on all channels
      static void SendSerialOutPanic(int delayBetweenChannelsMs=20)
      {
        digitalWrite(led1, LED_ON);
        TXLED1;
  
        for (uint8_t channel = 1; channel <= 16; channel++)
        {
          midiA.sendControlChange(AllSoundOff,          0,  channel);
          midiA.sendControlChange(ResetAllControllers,  0,  channel);
          delay(delayBetweenChannelsMs); // allow enough time old interfaces to handle the resets
        }
  
        digitalWrite(led1, LED_OFF);
        TXLED0;
      }
      //! Send to Midi Serial a basic note on then off of velocity 100 on first channel for testing
      static void SerialOutNote(byte pitch, int duration=250)
      {
        const uint8_t channel = 0, velocity = 100;
      
        midiA.sendNoteOn(pitch, velocity, channel);
        delay(duration);
        midiA.sendNoteOff(pitch, velocity, channel);
      
      }
      //! Send to Midi Usb a basic note on then off of velocity 100 on first channel for testing
      static void UsbOutNote(byte pitch, int duration=250)
      {
        const uint8_t channel = 0, velocity = 100;
        midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
        midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
        
        MidiUSB.sendMIDI(noteOn);
        MidiUSB.flush();
        delay(duration);
        MidiUSB.sendMIDI(noteOff);
        MidiUSB.flush();
      }
  
  };
}
