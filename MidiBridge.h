#pragma once
//
// MidiBridge Class declaration
//
// Author: Fabien
// License: GPL V3.0 © 2016 Fabien (https://github.com/fab672000)

#include "MIDI.h"
#include "MIDIUSB.h"
#include <stdint.h>
#include <stdio.h>

#define MIDIBRIDGE_CREATE_INSTANCE(Type, SerialPort, Name)  MidiBridge<Type> Name((Type&)SerialPort)

namespace midi
{

/**
  Utility class template to convert between serial and usb midi.
*/
template<typename SerialPort, typename Settings = DefaultSettings>
class MidiBridge : public MidiInterface<SerialPort, Settings>
{
public:

    MidiBridge(SerialPort &inSerial):MidiInterface<SerialPort, Settings>(inSerial){}

    bool SendToSerial(const midiEventPacket_t& rx);
    bool SendToUSB();

  //! Given a midiusb  data buffer, create and return the corresponding serial midi
  //! contiguous buffer for sending to uart. Caller must delete[] the returned value when done.
  static  uint8_t* GetUsbToSerialSysExBuffer(const uint8_t *data, size_t size, size_t& outSize);
  
  //! Given a serial midi data buffer, create and return the corresponding usb midi packets
  //! for sending to USB connection. Caller must delete[] the returned value when done.
  static  uint8_t* GetSerialToUsbSysExBuffer(const uint8_t* data, size_t size, size_t& outSize);
  
  //! Given a midiusb  data buffer, send the corresponding serial midi data to uart
  bool UsbToSerialSysEx(const uint8_t *data, size_t size);
  
  //! Given a serial midi data buffer, send the corresponding serial midi data to USB
  bool SerialToUsbSysEx(const uint8_t *data, size_t size);
};

// Template implementation
template<class SerialPort, class Settings>
uint8_t* MidiBridge<SerialPort, Settings>::GetUsbToSerialSysExBuffer(const uint8_t* data, size_t size, size_t& midiDataSize)
{
    if (data == nullptr || size == 0 || (size % 4) != 0) return nullptr; // usb packets must be DWORDs
    midiDataSize = (size >> 2) * 3;
    uint8_t lastPacketHdr = data[size - 4];
    if (lastPacketHdr > 4) midiDataSize -= (7 - lastPacketHdr); //  adjust size  if needed
    auto midiData = new uint8_t[midiDataSize];
    auto *p = midiData;
    auto bytesRemaining = midiDataSize;

    for (const uint8_t *d = data + 1; d<data + size; d += 4)
    {
        switch (bytesRemaining) {
            case 1:
                *p = d[0];		// SysEx ends with following single uint8_t
                break;
            case 2:
                *p++ = d[0];	// SysEx ends with following two uint8_ts
                *p = d[1];
                break;
            default:
                *p++ = d[0];	// SysEx with following three uint8_ts
                *p++ = d[1];
                *p++ = d[2];
                break;
        }
        bytesRemaining = bytesRemaining>3 ? bytesRemaining - 3 : 0;
    }
    return midiData; // it is caller responsibility to delete[] this data
}

template<class SerialPort, class Settings>
bool MidiBridge<SerialPort, Settings>::UsbToSerialSysEx(const uint8_t* data, size_t size)
{
    size_t midiDataSize;
    auto buffer = GetUsbToSerialSysExBuffer(data, size, midiDataSize);

    if (buffer == nullptr) return false;

    this->sendSysEx((unsigned)midiDataSize, buffer, true);
    delete[] buffer;

    return true;
}

template<class SerialPort, class Settings>
uint8_t* MidiBridge<SerialPort, Settings>::GetSerialToUsbSysExBuffer(const uint8_t* data, size_t size, size_t& outDataSize)
{
    if (data == nullptr || size == 0) return nullptr;

    outDataSize = (size + 2) / 3 * 4;
    uint8_t* midiData = new uint8_t[outDataSize];
    auto d = data;
    auto p = midiData;
    auto bytesRemaining = size;

    while (bytesRemaining > 0) {
        switch (bytesRemaining) {
            case 1:
                *p++ = 5;   // SysEx ends with following single uint8_t
                *p++ = *d;
                *p++ = 0;
                *p = 0;
                bytesRemaining = 0;
                break;
            case 2:
                *p++ = 6;   // SysEx ends with following two uint8_ts
                *p++ = *d++;
                *p++ = *d;
                *p = 0;
                bytesRemaining = 0;
                break;
            case 3:
                *p++ = 7;   // SysEx ends with following three uint8_ts
                *p++ = *d++;
                *p++ = *d++;
                *p = *d;
                bytesRemaining = 0;
                break;
            default:
                *p++ = 4;   // SysEx starts or continues
                *p++ = *d++;
                *p++ = *d++;
                *p++ = *d++;
                bytesRemaining -= 3;
                break;
        }
    }
    return midiData;
}

template<class SerialPort, class Settings>
bool MidiBridge<SerialPort, Settings>::SerialToUsbSysEx(const uint8_t* data, size_t size)
{
    size_t outSize = 0;
    auto buffer = GetSerialToUsbSysExBuffer(data, size, outSize);
    if (buffer == nullptr) return false;

    MidiUSB.write(data, size);
    delete[] buffer;
    return true;
}

template<class SerialPort, class Settings>
bool MidiBridge<SerialPort, Settings>::SendToSerial(const midiEventPacket_t &rx) {
    if (rx.header == 0) return false;
    else if(rx.header==0xF)
    {
        this->send((MidiType)rx.byte1, rx.byte2, rx.byte3, (Channel)0);
    }
    else if (rx.header>=0x8)
    {
        this->send((MidiType)(rx.byte1&0xf0), rx.byte2, rx.byte3, (Channel) (rx.byte1&0x0f)+1);
    }
    else if(rx.header>=0x4)
    {
        UsbToSerialSysEx((const byte*) &rx, sizeof(rx));
    }
    return true;
}

template<class SerialPort, class Settings>
bool MidiBridge<SerialPort, Settings>::SendToUSB()
{
    MidiType msgType  = this->getType();

    if (msgType==0) return false;
    else if(msgType==0xF0)
    {
        SerialToUsbSysEx(this->getSysExArray(), this->getSysExArrayLength());
    }
    else
    {
        midiEventPacket_t tx= 
        {
        	byte(msgType>>4), 
        	byte(msgType | ((this->getChannel()-1) & 0x0f)), /* getChannel() returns values from 1 to 16 */
        	this->getData1(), 
        	this->getData2()
        };
        MidiUSB.sendMIDI(tx);
    }
    return true;
}

}
