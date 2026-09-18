// DWT_Delay.c
#include "DWT_Delay.h"

#define DWT_CTRL   (*(volatile uint32_t *)0xE0001000)   // DWT Control Register
#define DWT_CYCCNT (*(volatile uint32_t *)0xE0001004)   // DWT Cycle Counter

// Initialize DWT
void DWT_Init(void) {
    if (!(DWT_CTRL & (1 << 0))) {       // Check if DWT is disabled
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // Enable trace
        DWT_CTRL = (DWT_CTRL | (1 << 0));            // Enable DWT cycle counter
        DWT_CYCCNT = 0;                  // Reset counter
    }
}

// Delay in microseconds (µs)
void DWT_Delay_us(uint32_t microseconds) {
    uint32_t start = DWT_CYCCNT;
    uint32_t cycles = microseconds * (SystemCoreClock / 1000000); // Convert µs to CPU cycles
    while ((DWT_CYCCNT - start) < cycles); // Wait until cycle count is reached
}

// Delay in milliseconds (ms)
void DWT_Delay_ms(uint32_t milliseconds) {
    DWT_Delay_us(milliseconds * 1000);   // Convert ms to µs
}
