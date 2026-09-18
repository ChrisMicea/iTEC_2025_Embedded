// DWT_Delay.h
#ifndef DWT_DELAY_H
#define DWT_DELAY_H

#include "stm32f4xx_hal.h"  // Adjust for your STM32 series (e.g., stm32f1xx.h for F1)

void DWT_Init(void);                     // Initialize DWT
void DWT_Delay_us(uint32_t microseconds); // Microsecond delay
void DWT_Delay_ms(uint32_t milliseconds); // Millisecond delay

#endif
