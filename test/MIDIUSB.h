#pragma once
#include "MIDI.h"
typedef unsigned char byte;

// mocked MIDIUSB inetrface
struct midiEventPacket_t
{
	byte header;
	byte byte1;
	byte byte2;
	byte byte3;

};

class _MIDIUSB
{
public:
    inline void write(const byte* buffer, unsigned size) {}
    inline void sendMIDI(const midiEventPacket_t& tx) {}
};

extern _MIDIUSB MidiUSB;
