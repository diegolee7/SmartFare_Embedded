/*
===============================================================================
 Name        : RFID_LIB.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "chip.h"
#include "board.h"
#include <cr_section_macros.h>

// TODO: insert other include files here
#include "delay.h"
// TODO: insert other definitions and declarations here

int main(void) {


    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();

    // TODO: insert code here
    SysTick_Init();//to use the daley libray

    while(1) {
		delay_ms(2000);
		Board_LED_Toggle(0);
		DEBUGOUT("TEST OUTPUT");
    	__WFI();
    }
    return 0 ;
}
