/**
 * LICENSE
 * Main file ,incremental functionalities added
 * 1- Show message in the OM13082 shield LCD when a button is pressed
 * 2- Use one RFID on SSP1
 * 3- Use two  RFID readers on SSP0
 */
/*****************************************************************************
 * LPCXpresso4337 project includes
 ****************************************************************************/
#include "board.h"
#include "chip.h"
#include <cr_section_macros.h>

/*****************************************************************************
 * Custom files and libraries includes
 ****************************************************************************/
#include "lcd_shield.h"
#include "delay.h"
#include "MFRC522.h"
#include "SmartFareData.h"

#include "RFIDUtils.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define  USER_BUFFER_SIZE  10

#if defined(BOARD_NXP_LPCXPRESSO_4337)

/* The only switch available on this board is the ISP switch input.  The other
   switches are connected to an I2C device. */
#define TEST_BUTTON_PORT            0
#define TEST_BUTTON_BIT             7
#define TEST_BUTTON_PIN_PORT        2
#define TEST_BUTTON_PIN_BIT         7
#define TEST_BUTTON_MODE_FUNC       SCU_MODE_FUNC0


#else
#error "Grouped GPIO Interrupt not configured for this example"
#endif /* defined(BOARD_HITEX_EVA_4350) || defined(BOARD_HITEX_EVA_1850) */

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

int message_code = 0;
int interrupt_flag = 0;
int last_balance = 0;
unsigned int last_user_ID;
int usersBufferIndex=0;

//buffer to store the active users in the system. Onboard passengers
static  UserInfo_T usersBuffer[USER_BUFFER_SIZE];

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle Group GPIO 0 interrupt
 * @return	Nothing
 */
void GINT0_IRQHandler(void)
{
	Chip_GPIOGP_ClearIntStatus(LPC_GPIOGROUP, 0);
	interrupt_flag = 1;
}


/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void)
{
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();

	/* Board_Init calls Chip_GPIO_Init and enables GPIO clock if needed,
	   Chip_GPIO_Init is not called again */
	Board_Init();
	Board_LED_Set(0, false);

	//init shield lcd, and SSP interface pins
	board_lcd_init();//


	/*****************************************************************************
	 *Set up RFID
	 ****************************************************************************/
	    SysTick_Init();//to use the delay library
	    MFRC522Ptr_t mfrc1=MFRC522_Init();
	    //Define the pins to use as CS(SS or SSEL)  an RST
		Chip_SCU_PinMuxSet (0x1, 12,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)); //Set as GPIO
		Chip_SCU_PinMuxSet (0x1, 10,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)); //Set as GPIO
	    // GPIO1[12]= P2_12
	    mfrc1->_chipSelectPin.port = 1;
	    mfrc1->_chipSelectPin.pin = 12;
	    // GPIO1[10]= P2_9
	    mfrc1->_resetPowerDownPin.port = 1;
	    mfrc1->_resetPowerDownPin.pin = 10;
	    PCD_Init(mfrc1,LPC_SSP0);
	    DEBUGOUT("Reader 1 ");
	    PCD_DumpVersionToSerial(mfrc1);	// Show details of PCD - MFRC522 Card Reader details

	    //Repeat config for RFID reader 2
	    MFRC522Ptr_t mfrc2=MFRC522_Init();
	    //Define the pins to use as CS(SS or SSEL)  an RST
		Chip_SCU_PinMuxSet (0x1, 0,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)); //Set as GPIO
		Chip_SCU_PinMuxSet (0x5, 02,  (SCU_PINIO_FAST | SCU_MODE_FUNC4)); //Set as GPIO
	    // GPIO1[0]= P1_07
	    mfrc2->_chipSelectPin.port = 1;
	    mfrc2->_chipSelectPin.pin = 0;
	    // GPIO5[02]= P2_02
	    mfrc2->_resetPowerDownPin.port = 5;
	    mfrc2->_resetPowerDownPin.pin = 2;
	    PCD_Init(mfrc2,LPC_SSP0);
	    DEBUGOUT("Reader 2 ");
	    PCD_DumpVersionToSerial(mfrc2);	// Show details of PCD - MFRC522 Card Reader details

	    //auxiliary variable to search an userId in the usersBuffer
	    int userIndex;

	/* Set pin back to GPIO (on some boards may have been changed to something
	   else by Board_Init()) */
	Chip_SCU_PinMuxSet(TEST_BUTTON_PIN_PORT, TEST_BUTTON_PIN_BIT,
					   (SCU_MODE_INBUFF_EN | SCU_MODE_INACT | TEST_BUTTON_MODE_FUNC) );

	/* Group GPIO interrupt 0 will be invoked when the button is pressed. */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, TEST_BUTTON_PORT, TEST_BUTTON_BIT);
	Chip_GPIOGP_SelectLowLevel(LPC_GPIOGROUP, 0, TEST_BUTTON_PORT, 1 << TEST_BUTTON_BIT);
	Chip_GPIOGP_EnableGroupPins(LPC_GPIOGROUP, 0, TEST_BUTTON_PORT, 1 << TEST_BUTTON_BIT);
	Chip_GPIOGP_SelectAndMode(LPC_GPIOGROUP, 0);
	Chip_GPIOGP_SelectEdgeMode(LPC_GPIOGROUP, 0);

	/* Enable Group GPIO interrupt 0 */
	NVIC_EnableIRQ(GINT0_IRQn);


	/* Spin in a loop here.  All the work is done in ISR. */
	while (1) {
		if (interrupt_flag) {
			change_lcd_message(message_code);
			interrupt_flag = 0;
			message_code++;
			if (message_code > 4) {
				message_code = 0;
			}
		}

		//////////////////////
	    //RF ID test
		//////////////////////

		// Look for new cards
		if ( ! PICC_IsNewCardPresent(mfrc1)) {
					Board_LED_Toggle(1);
					// delay_ms(500);
					continue;
		}

		// Select one of the cards
		if ( ! PICC_ReadCardSerial(mfrc1)) {
					Board_LED_Toggle(2);
					// delay_ms(500);
					continue;
		}

		//show card UID
		DEBUGOUT("Card uid: ");
		for (uint8_t i = 0; i < mfrc1->uid.size; i++)
		 {
			DEBUGOUT(" %X", mfrc1->uid.uidByte[i]);
		 }
		DEBUGOUT("\n\r");

		//convert the uid bytes to an integer, byte[0] is the MSB
		last_user_ID= (int) mfrc1->uid.uidByte[3] | (int) mfrc1->uid.uidByte[2] <<8 | (int) mfrc1->uid.uidByte[1]<<16 | (int) mfrc1->uid.uidByte[0]<<24;
		
		//search for the uID in the usersBuffer
		userIndex = getUserByID(last_user_ID);
		if(userIndex == -1){
			//Register user in the buffer
			addNewUser(last_user_ID);
		}
		else{
			//user is already onboard
			// change_lcd_message(USTATUS_UNAUTHORIZED);
		}


		//read the user balance
		last_balance=readCardBalance(mfrc1);
		if(last_balance == (-999)){
			//error handling
		}
		else{
			//check for minumim balance
			if (last_balance < min_balance) {
				// change_lcd_message(USTATUS_INSUF_BALANCE);
			}
			else{
				// change_lcd_message(USTATUS_AUTHORIZED);
			}
		}
		__WFI();
	}
}


/**********************************
 *  Some util functions
 **********************************/

/**
 * Search for a given userID, returns an index to it if found, -1 otherwise
 * @param  userID the iD to search for
 * @return        the index of the user in the usersBuffer
 */
int getUserByID(unsigned int userID){

	int i ;

	for ( i = 0; i<USER_BUFFER_SIZE ; i++){
		if ( userID == usersBuffer[i].userID){
			return i;
		}
	}

	return -1;
}

void addNewUser(unsigned int userID){
	UserInfo_T new_user;
	new_user.userID= userID;

	//add user to usersBuffer
	usersBuffer[usersBufferIndex]=new_user;
	usersBufferIndex++;
	if(usersBufferIndex > USER_BUFFER_SIZE-1){
		//save buffer in other safe place
		//overwrite buffer values
		usersBufferIndex = 0;
	}
}


