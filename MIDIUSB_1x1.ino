#define ARDUINO_MAX_POWER_MA 20
/*
 * MIDIUSB 1x1 interface, works with iPad.
 * 
 *   Important Note: needs D_CONFIG macro in USBCore.h to be patched to change USB_CONFIG_POWER_MA(500) 
 *     with a lower value (i.e 20) in order to work with iPad, as it is bus powered by the tablet.
 * 
 * Created: October, 11 2016
 * Author: Fabien
 * License: GPL V3 © 2016 Fabien (https://github.com/fab672000)
 */

#include <avr/power.h>
#include "TimerOne.h"
#include "MIDIUSB.h"
#include "MIDI.h"
#include "MidiBridge.h"
#include "Leds.h"

USING_NAMESPACE_MIDI

//------------------------------------------------------------------------------
const byte   led1 = LED;
const byte   led2 = TXLED1;

static bool timer_elapsed = false;

// WARNING on leonardo / Pro Micro serial is Serial1 !!, Serial is USB
MIDIBRIDGE_CREATE_INSTANCE(HardwareSerial, Serial1,     midiA);

//------------------------------------------------------------------------------
void power_save()
{ // Disable ADC, SPI, I2C, Timer 2 and Timer 3
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();

  power_timer2_disable();
  power_timer3_disable();
}
//------------------------------------------------------------------------------
void leds_off()
{
  digitalWrite(led1, LED_OFF);
  TXLED1;
}
//------------------------------------------------------------------------------
void note(byte pitch, int duration=250)
{
  const uint8_t channel =1, velocity = 100;

  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
  MidiUSB.flush();
  //midiA.sendNoteOn(pitch, velocity, channel);

  delay(duration);
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
  MidiUSB.flush();
  //midiA.sendNoteOff(pitch, velocity, channel);

}
//------------------------------------------------------------------------------
void timer1_callback()
{
  timer_elapsed = true;
}
//------------------------------------------------------------------------------
// Midi Interface Setup: initialize leds, serial midi, timer and power save 
//------------------------------------------------------------------------------
void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  // briefly blink leds as an alive signal
  for (int i=0; i< 2; i++) {
   delay(150);
   digitalWrite(led1, LED_ON);
   TXLED1;
   delay(150);
   digitalWrite(led1, LED_OFF);
   TXLED0;
  }
  leds_off();
  
  midiA.begin(MIDI_CHANNEL_OMNI);
  
  // Set timer to save even more power after a period of inactivity
  Timer1.initialize(500000); // initialize timer1, and set a period in us
  Timer1.attachInterrupt(timer1_callback);

  power_save(); // save extra millamps by disabling what we do not use
}
//------------------------------------------------------------------------------
// Main loop: Handle serial rx first and send it to host if any, 
//   then read host and send to serial if any.
//------------------------------------------------------------------------------
void loop() {
  bool sent = false, recv=false;
  
  // Handle serial midi messages, if any ; send them to host:
  while (midiA.read())
  {
      sent = midiA.SendToUSB();
      // state = state==LED_OFF ? LED_ON : LED_OFF;
  }
  if(sent) {MidiUSB.flush();}
  
  // Handle host messages, if any ; send them to serial:
  midiEventPacket_t rx;
  do 
  {
    rx = MidiUSB.read();
    midiA.SendToSerial(rx);
  } while (rx.header != 0);

  // Handle timer and leds to save power while no midi is received / sent
  if(sent || recv)
  {
      timer_elapsed = sent = recv = false;
      Timer1.restart();
  }
  else if(timer_elapsed)
  {
    leds_off();
    timer_elapsed = false;
  }
}
//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
