/**
 * LICENSE
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
#include "SIM800.h"
#include "rfid_utils.h"
#include "MFRC522.h"

/**********************************
 *  Extra functions defined in the main.c file
 **********************************/
void setupGSM();
void setupRFID();
void userTapIn();
void userTapOut();
int getUserByID(unsigned int userID);
void addNewUser(unsigned int userID);

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
// temporary variables
int last_balance = 0;
unsigned int last_user_ID;
int usersBufferIndex = 0;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

// RFID structs
MFRC522Ptr_t mfrc1;
MFRC522Ptr_t mfrc2;

// buffer to store the active users in the system. Onboard passengers
static UserInfo_T usersBuffer[USER_BUFFER_SIZE];

/**
 * @brief	Main program body
 * @return	Does not return
 */
int main(void) {
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();

	// To use the delay library
	SysTick_Init();

	/* Board_Init calls Chip_GPIO_Init and enables GPIO clock if needed,
	   Chip_GPIO_Init is not called again */
	Board_Init();
	Board_LED_Set(0, false);

	// Init shield lcd, and SSP interface pins
	board_lcd_init(); //

	//setupGSM();
	setupRFID1_entrance(mfrc1);
	//setupRFID2_exit(mfrc2);

	change_lcd_message(START_MESSAGE);

	// Every LCD message changes the SSP configuration, must configure it for
	// the RFID again
	PCD_Init(mfrc1, LPC_SSP1);

	while (1) {
		// Look for new cards in RFID1
		if (PICC_IsNewCardPresent(mfrc1)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc1)) {
				userTapIn();
			}
		}

		/*
		// Look for new cards in RFID2
		if (PICC_IsNewCardPresent(mfrc2)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc2)) {
				userTapOut();
			}
		}
		*/

		// Calculate fare based on vehicle movement
		// Update user data
		__WFI();
	}
}

/**********************************
 *  Peripheral setup functions
 **********************************/

void setupGSM() {
	uint8_t ret;
	for (uint8_t i = 0; i < MAX_ATTEMPTS; i++) {
		DEBUGOUT("\n**************\nInitializing SIM800");
		initSIM800();
		DEBUGOUT("\nSetup SIM800");
		ret = setupSIM800();
		if (ret == 0) {
			break;
		}
		DEBUGOUT("\nError: %d", ret);
		DEBUGOUT("\nError Buffer: %s", bufferSIM800);
	}
	DEBUGOUT("\nSetup Successful");
}

/**********************************
 *  System routine functions
 **********************************/

void userTapIn() {

	// // show card UID
	// DEBUGOUT("Card uid: ");
	// for (uint8_t i = 0; i < mfrc1->uid.size; i++) {
	// 	DEBUGOUT(" %X", mfrc1->uid.uidByte[i]);
	// }
	// DEBUGOUT("\n\r");

	// Convert the uid bytes to an integer, byte[0] is the MSB
	last_user_ID =
		(int)mfrc1->uid.uidByte[3] | (int)mfrc1->uid.uidByte[2] << 8 |
		(int)mfrc1->uid.uidByte[1] << 16 | (int)mfrc1->uid.uidByte[0] << 24;

	// Search for the uID in the usersBuffer
	int userIndex = getUserByID(last_user_ID);
	if (userIndex == -1) {
		// Register user in the buffer
		addNewUser(last_user_ID);
	} else {
		// user is already onboard
		change_lcd_message(USTATUS_UNAUTHORIZED);
		PCD_Init(mfrc1, LPC_SSP1);
	}

	// Read the user balance
	last_balance = readCardBalance(mfrc1);
	if (last_balance == (-999)) {
		// Error handling, the card does not have proper balance data inside
	} else {
		// Check for minumim balance
		if (last_balance < min_balance) {
			change_lcd_message(USTATUS_INSUF_BALANCE);
			PCD_Init(mfrc1, LPC_SSP1);
		} else {
			set_lcd_last_userID(last_user_ID);
			set_lcd_balance(last_balance);
			change_lcd_message(USTATUS_AUTHORIZED);
			PCD_Init(mfrc1, LPC_SSP1);
		}
	}
}

void userTapOut() {
	// TODO
}

/**********************************
 *  Some util functions
 **********************************/

/**
 * Search for a given userID, returns an index to it if found, -1 otherwise
 * @param  userID the iD to search for
 * @return        the index of the user in the usersBuffer
 */
int getUserByID(unsigned int userID) {

	int i;

	for (i = 0; i < USER_BUFFER_SIZE; i++) {
		if (userID == usersBuffer[i].userID) {
			return i;
		}
	}

	return -1;
}

/**
 * Create an UserInfo_T instance with empty data and stores it int the
 * usersBuffer
 * @param userID user unique identification number in the system
 */
void addNewUser(unsigned int userID) {
	UserInfo_T new_user;
	new_user.userID = userID;

	// add user to usersBuffer
	usersBuffer[usersBufferIndex] = new_user;
	usersBufferIndex++;
	if (usersBufferIndex > USER_BUFFER_SIZE - 1) {
		// save buffer in other safe place
		// overwrite buffer values
		usersBufferIndex = 0;
	}
}
