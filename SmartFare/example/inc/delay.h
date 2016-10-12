/**
 * Simple delay library
 * Based on code from http://patrickleyman.be/blog/stm32f407-delay-with-systick/
 *
 * To use it include the interruption handler in this way:
 * void SysTick_Handler(void) {
        TimeTick_Decrement();
    }

    Then just call the functions inside your code
 */

#ifndef __DELAY_LIB_H
#define __DELAY_LIBH

#define TICKRATE_HZ (1000)  /* 1000 ticks per second */


#include "board.h"

static volatile uint32_t sysTickCounter;

void SysTick_Init(void);
void delay_ms(uint32_t delayTime);
void SysTick_Handler(void);

#endif
