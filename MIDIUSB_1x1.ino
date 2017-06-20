/*
 * MIDIUSB 1x1 interface, works with iPad.
 * 
 *   Currently tested boards are Arduino Micro, Leonardo.
 *   Not tested on Due but should work as well.
 * 
 *   Important Note: needs D_CONFIG macro in USBCore.h to be patched to change USB_CONFIG_POWER_MA(500) 
 *     with a lower value (i.e 20) in order to work with iPad, as it is bus powered by the tablet.
 * 
 * Created: October, 11 2016
 * Author: Fabien
 * License: GPL V3 © 2016 Fabien (https://github.com/fab672000)
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "MIDIUSB.h"
#include "MIDI.h"
#include "MidiBridge.h"
#include "Power.h"
#include "Leds.h"

USING_NAMESPACE_MIDI

#define PANIC_BUTTON_PIN  2
#include "Buttons.h"

// WARNING on leonardo / Pro Micro serial is Serial1 !!, Serial is USB
MIDIBRIDGE_CREATE_INSTANCE(HardwareSerial, Serial1,     midiA);
#include "MidiUtils.h"


//------------------------------------------------------------------------------
static bool timer_elapsed = false;
//------------------------------------------------------------------------------
static void idle_timer_callback()
{
  timer_elapsed = true;
}
//------------------------------------------------------------------------------
// Midi Interface Setup: initialize leds, serial midi, timer and power save 
//------------------------------------------------------------------------------
void setup() 
{
  Buttons::Setup();
  Leds::Setup();
  Leds::AliveSignal(); // briefly blink leds as an alive signal
  
  midiA.begin(MIDI_CHANNEL_OMNI);
  Power::SetupIdleTimer(idle_timer_callback);  
  Power::PowerSaveMode(); // save extra millamps by disabling what we do not use
}
//------------------------------------------------------------------------------
// Main loop: Handle serial rx first and send it to host if any, 
//   then read host and send to serial if any.
//------------------------------------------------------------------------------
void loop() 
{
  bool sent = false, recv=false;
  
  // Handle serial midi messages, if any ; send them to host:
  while (midiA.read())
  {
      sent = midiA.SendToUSB();
  }
  if(sent) {MidiUSB.flush();}
  
  // Handle host messages, if any ; send them to serial:
  midiEventPacket_t rx;
  do 
  {
    rx = MidiUSB.read();
    recv = midiA.SendToSerial(rx);
  } while (rx.header != 0);

  // Handle timer and leds to save power while no midi is received / sent
  if(sent || recv)
  {
      timer_elapsed = sent = recv = false;
      Power::RestartIdleTimer();
  }
  else if(timer_elapsed)
  {
    Leds::AllLedsOff();
    timer_elapsed = false;
  }

  // put off all notes and reset controllers
  if (Buttons::Pressed(PANIC_BUTTON_PIN)) 
  {
    MidiUtils::SendSerialOutPanic();
    Buttons::WaitUntilReleased(PANIC_BUTTON_PIN);
  }
}
//------------------------------------------------------------------------------
// EOF
//------------------------------------------------------------------------------
