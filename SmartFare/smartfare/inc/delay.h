/**
 *  delay.h
 *
 *  Created on: 2016
 *      Author: Luis Fernando Guerreiro
 *      
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *	 Check "LICENSE" file in the root folder of project
 * 
 * Simple delay library based on public domain code from 
 * 	http://patrickleyman.be/blog/stm32f407-delay-with-systick/
 *
 * To use it include the interrupt handler in this way:
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

/**
 * Enables the the Systick interrupt handler in the system
 */
void SysTick_Init(void);

/**
 * Delay processor by using an empty loop
 * @param delayTime time in milliseconds
 */
void delay_ms(uint32_t delayTime);

/**
 * Decrease the sysTickCounter at every interrupt handler call
 */
void SysTick_Handler(void);

#endif
