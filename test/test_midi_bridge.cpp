// testmd01.cpp : test midi bridge API
//
#include "gtest/gtest.h"
#include "../MidiBridge.h"
#include "MIDI.h"
#include "MIDIUSB.h"
#include <vector>

using namespace midi;
_MIDIUSB MidiUSB;

typedef std::vector<uint8_t> SysExBufferType;
typedef MidiBridge<HardwareSerial> _MIDI;
HardwareSerial hardwareSerial;

// Test DATA
static SysExBufferType SysExDatasets[] =
        {
                { 0xF7 },
                { 0xF0, 0xF7 },
                { 0xF0, 0x01, 0xF7 },
                { 0xF0, 0x01, 0x02, 0xF7 },
                { 0xF0, 0x01, 0x02, 0x03, 0xF7 },
                { 0xF0, 0x01, 0x02, 0x03, 0x04, 0xF7 },
                { 0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0xF7 },
                { 0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0xF7 },
        };

int findCorrespondingUsbBufferIndexFrom(unsigned  i)
{
    int correspondingOutputIndex = i / 3 * 4 + i % 3 + 1;
    return correspondingOutputIndex;
}

TEST(TestSerialToUsbSysEx, TestInvalidInput)
{
    size_t outSize = 0;
    _MIDI sut(hardwareSerial);
    EXPECT_EQ(false, sut.SerialToUsbSysEx(nullptr, 0));
    EXPECT_EQ(false, sut.SerialToUsbSysEx(&SysExDatasets[0][0], 0));
}

TEST(TestSerialToUsbSysEx, TestUnwrapping)
{
    for (auto sysExTestDataList : SysExDatasets)
    {
        auto originalSize = sysExTestDataList.size();
        size_t outSize = 0;

        auto buffer = _MIDI::GetSerialToUsbSysExBuffer(&sysExTestDataList[0], originalSize, outSize);

        EXPECT_NE(nullptr, buffer);
        EXPECT_EQ(0, outSize % 4);
        for (unsigned i = 0; i < originalSize; i++)
        {
            EXPECT_EQ(sysExTestDataList[i], buffer[findCorrespondingUsbBufferIndexFrom(i)]);
        }
        EXPECT_EQ(0xF7, buffer[findCorrespondingUsbBufferIndexFrom(originalSize - 1)]);

        delete[] buffer;
    }
}

TEST(TestUsbToSerialSysEx, TestUnwrapping)
{
    for (auto sysExTestDataList : SysExDatasets)
    {
        auto originalSize = sysExTestDataList.size();
        size_t outSize = 0;
        auto buffer = _MIDI::GetSerialToUsbSysExBuffer(&sysExTestDataList[0], originalSize, outSize);

        EXPECT_NE(nullptr, buffer);
        EXPECT_EQ(0, outSize % 4);

        size_t outSize2 = 0;
        auto buffer2 = _MIDI::GetUsbToSerialSysExBuffer(buffer, outSize, outSize2);

        for (unsigned i = 0; i < originalSize; i++)
        {
            EXPECT_EQ(sysExTestDataList[i], buffer2[i]);
        }
        EXPECT_EQ(0xF7, buffer2[originalSize- 1]);

        delete[] buffer;
        delete[] buffer2;
    }
}

TEST(TestUsbToSerialSysEx, TestInvalidInput)
{
    size_t outSize = 0;
    _MIDI sut(hardwareSerial);

    EXPECT_EQ(false, sut.UsbToSerialSysEx(nullptr, 0));
    EXPECT_EQ(false, sut.UsbToSerialSysEx(&SysExDatasets[0][0], 0));
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

