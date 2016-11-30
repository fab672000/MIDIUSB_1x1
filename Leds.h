#pragma once
//------------------------------------------------------------------------------
// Generic led pin and level mapping, defines LED, LED_OFF, LED_ON
// Author: Fabien 
// License: GPL V3 © 2016 Fabien (https://github.com/fab672000)
 //-----------------------------------------------------------------------------
#if defined(ARDUINO_SAM_DUE)
//#pragma message("Due")
  const byte LED = 13;
#elif defined(STM32_MCU_SERIES)
//#pragma message("STM32 MCU")
  const byte LED = PC13;
#elif defined(ARDUINO_AVR_MICRO)
//#pragma message("Micro")
  const byte LED = 17; // TX led on micro
#elif defined(ARDUINO_AVR_PRO) || defined(ARDUINO_AVR_MINI)
//#pragma message("Mini")
  const byte LED = 13; // TX led on micro
#elif defined(ARDUINO_AVR_LEONARDO)
//#pragma message("Leonardo")
  const byte LED = 13; // TX led on micro
#elif defined(ESP8266_ESP12)
//#pragma message("NodeMCU 1.0")
  const byte LED = 13; // TX led on micro
#else 
//#pragma message("Default")
  const byte LED = 17;
#endif
#if defined(STM32_MCU_SERIES) || defined(ARDUINO_AVR_MICRO)
# define LED_ON   LOW
# define LED_OFF HIGH
#else
# define LED_OFF  LOW
# define LED_ON  HIGH
#endif  

const int LED_DELAY_MS = 500;

class Leds
{
 public:
  static void blink(byte led, int del=LED_DELAY_MS)
  {
    digitalWrite(led, LED_ON);  
    delay(del);
    digitalWrite(led, LED_OFF); 
    delay(del);
  }
};

