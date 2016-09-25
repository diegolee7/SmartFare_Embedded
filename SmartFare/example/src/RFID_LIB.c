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

// main function for tests
//int main(void) {
//
//
//    // Read clock settings and update SystemCoreClock variable
//    SystemCoreClockUpdate();
//    // Set up and initialize all required blocks and
//    // functions related to the board hardware
//    Board_Init();
//
//    // TODO: insert code here
//    SysTick_Init();//to use the delay library
//    MFRC522Ptr_t mfrc1=MFRC522_Init();
//    //Define the pins to use as CS(SS or SSEL)  an RST
//	  Chip_SCU_PinMuxSet (0x1, 11,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)); Set as GPIO
//	  Chip_SCU_PinMuxSet (0x1, 10,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)); Set as GPIO
//    // GPIO1[11]= P2_12
//    mfrc1->_chipSelectPin.port = 1;
//    mfrc1->_chipSelectPin.pin = 11;
//    // GPIO1[10]= P2_9
//    mfrc1->_resetPowerDownPin.port = 1;
//    mfrc1->_resetPowerDownPin.pin = 10;
//    PCD_Init(mfrc1);
//    PCD_DumpVersionToSerial(mfrc1);	// Show details of PCD - MFRC522 Card Reader details
//
//    while(1) {
//		delay_ms(2000);
//		Board_LED_Toggle(0);
////		DEBUGOUT("TEST OUTPUT");
//		// Look for new cards
//		if ( ! PICC_IsNewCardPresent(mfrc1)) {
//			Board_LED_Toggle(1);
//			DEBUGOUT("NEWCARD\r\n");
////			return;
//		}
//
//		// Select one of the cards
//		if ( ! PICC_ReadCardSerial(mfrc1)) {
//			Board_LED_Toggle(2);
//			DEBUGOUT("READCARDSERIAL\r\n");
////			return;
//		}
//
//		// Dump debug info about the card; PICC_HaltA() is automatically called
//		PICC_DumpToSerial(mfrc1, &(mfrc1->uid));
//    	__WFI();
//    }
//    return 0 ;
//}
