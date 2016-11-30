#pragma once
#include <stdint.h>

typedef uint8_t MidiType;
typedef uint8_t Channel;
typedef uint8_t DataByte;

class DefaultSettings {};
class HardwareSerial {};

// mocked version of the MIDI lib for testing
template<class SerialPort, class Settings = DefaultSettings>
class MidiInterface
{
public:
	MidiInterface(SerialPort& inSerial) {}
	void sendSysEx(unsigned midiDataSize, uint8_t *midiData, bool headers_included){}
	void send(MidiType inType,
			  DataByte inData1,
			  DataByte inData2,
			  Channel inChannel){}
	MidiType getType();
	Channel getChannel();
	DataByte getData1();
	DataByte getData2();
	const uint8_t *getSysExArray();
	size_t getSysExArrayLength();
};


template<class SerialPort, class Settings>
MidiType MidiInterface<SerialPort, Settings>::getType()
{
	return 0;
}

template<class SerialPort, class Settings>
const uint8_t *MidiInterface<SerialPort, Settings>::getSysExArray()
{
	return nullptr;
}

template<class SerialPort, class Settings>
size_t MidiInterface<SerialPort, Settings>::getSysExArrayLength()
{
	return 0;
}

template<class SerialPort, class Settings>
Channel MidiInterface<SerialPort, Settings>::getChannel()
{
	return 1;
}

template<class SerialPort, class Settings>
DataByte MidiInterface<SerialPort, Settings>::getData1()
{
	return 1;
}

template<class SerialPort, class Settings>
DataByte MidiInterface<SerialPort, Settings>::getData2()
{
	return 2;
}
