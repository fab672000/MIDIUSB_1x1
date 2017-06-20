#pragma once
// Power Class declaration
//
// Author: Fabien
// License: GPL V3.0 © 2016 Fabien (https://github.com/fab672000)

#if !defined(ARDUINO_SAM_DUE)
# include <avr/power.h>
# include "TimerOne.h"
#endif

class Power
{
  public:
  
    static void SetupIdleTimer(void (*cb)() )
    { 
#if !defined(ARDUINO_SAM_DUE)
      // Set timer to save even more power after a period of inactivity
      Timer1.initialize(500000); // initialize timer1, and set a period in us
      Timer1.attachInterrupt(cb);
#endif
    }

    static void RestartIdleTimer()
    { 
#if !defined(ARDUINO_SAM_DUE)
      Timer1.restart();
#endif
    }
    
    static void PowerSaveMode()
    { 
#if !defined(ARDUINO_SAM_DUE)
      // Disable ADC, SPI, I2C, Timer 2 and Timer 3
      power_adc_disable();
      power_spi_disable();
      power_twi_disable();
    
      power_timer2_disable();
      power_timer3_disable();
#endif
    }
};
