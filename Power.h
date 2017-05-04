#pragma once
// Power Class declaration
//
// Author: Fabien
// License: GPL V3.0 © 2016 Fabien (https://github.com/fab672000)

#include <avr/power.h>

class Power
{
  public:
  
    static void PowerSaveMode()
    { // Disable ADC, SPI, I2C, Timer 2 and Timer 3
      power_adc_disable();
      power_spi_disable();
      power_twi_disable();
    
      power_timer2_disable();
      power_timer3_disable();
    }
};
