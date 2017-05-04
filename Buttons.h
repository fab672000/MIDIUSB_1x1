#pragma once
// Buttons Class declaration
//
// Author: Fabien
// License: GPL V3.0 © 2016 Fabien (https://github.com/fab672000)

/**
 * Utility class for the midi interface, 
 *  for now only handling a panic button (uses INPUT_PULLUP pin mode)
 */
class Buttons
{
  public:
  
  //! Buttons setup, user must declare PANIC_BUTTON_PIN pin 
  static void Setup()
  {
    pinMode(PANIC_BUTTON_PIN, INPUT_PULLUP); // 10k internal pull-up for easy
  }
  
  static bool Pressed(uint8_t pin)  { return digitalRead(pin)==LOW; }

  static bool WaitUntilReleased(uint8_t pin) 
  { 
    while(Buttons::Pressed(PANIC_BUTTON_PIN)) delay(20);
  }
};


