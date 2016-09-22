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
#include "MFRC522.h"
// TODO: insert other definitions and declarations here

int main(void) {


    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();

    // TODO: insert code here
    SysTick_Init();//to use the delay library
    MFRC522Ptr_t mfrc1;
    MFRC522_Init(mfrc1);
    //Define the pins to use as CS(SS or SSEL)  an RST
    // GPIO1[8]= P1_5
    mfrc1->_chipSelectPin.port = 1;
    mfrc1->_chipSelectPin.pin = 8;
    // GPIO3[4]= P6_5
    mfrc1->_resetPowerDownPin.port = 3;
    mfrc1->_resetPowerDownPin.pin = 4;
    PCD_Init(mfrc1);

    while(1) {
		delay_ms(2000);
		Board_LED_Toggle(0);
		DEBUGOUT("TEST OUTPUT");
    	__WFI();
    }
    return 0 ;
}
