/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "oledc.h"
#include "tc275_spi_wrap.h"

inline void smallest_delay(void) {
  for (volatile uint32_t i=0; i < 1; i++) {
    ;
  }
}

void spi_master_write(uint8_t *data_ptr, uint8_t len) {
  spi_write(data_ptr, len);
}

void digital_out_write(uint8_t pin, uint8_t val) {
  switch (pin) {
    case OLEDC_PIN_EN: // EN / INT_2 / P02_6
      if (val > 0)
          IfxPort_setPinHigh(&MODULE_P02, 6);
      else
          IfxPort_setPinLow(&MODULE_P02, 6);
      break;
    case OLEDC_PIN_DC: // DC / PWM / P13_2
        if (val > 0)
            IfxPort_setPinHigh(&MODULE_P13, 2);
        else
            IfxPort_setPinLow(&MODULE_P13, 2);
      break;
    case OLEDC_PIN_RST: // RST / P10_4
        if (val > 0)
            IfxPort_setPinHigh(&MODULE_P10, 4);
        else
            IfxPort_setPinLow(&MODULE_P10, 4);
      break;
    case OLEDC_PIN_RW: // RW / AN / SAR4_7_AD0
    default:
    ;
  }
  smallest_delay();
}

void digital_out_high(uint8_t pin) {
    digital_out_write(pin, 1);
}


void digital_out_low(uint8_t pin) {
    digital_out_write(pin, 0);
}

/* [] END OF FILE */
