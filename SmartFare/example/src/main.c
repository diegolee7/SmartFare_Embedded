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
#include "rtc.h"
#include "jsonGenerator.h"

/**********************************
 *  Extra functions defined in the main.c file
 **********************************/
void setupGSM();
void setupRFID();
void userTapIn();
void userTapOut();
int getUserByID(unsigned int userID);
void addNewUser(unsigned int userID);
void saveTapInData();

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
// temporary variables
int last_balance = 0;
unsigned int last_user_ID;
int usersBufferIndex = 0;

//Temporary variables
unsigned int userID_1 = 2092088836;
unsigned int userID_2 = 1404324234;
uint8_t tapin_flag = 0;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

// RFID structs
MFRC522Ptr_t mfrc1;
MFRC522Ptr_t mfrc2;

// buffer to store the active users in the system. Onboard passengers
static UserInfo_T usersBuffer[USER_BUFFER_SIZE];

static char jsonString[400];
static UserInfo_T userInfo;

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
	setupRTC();
	//setupRFID1_entrance(&mfrc1);
	setupRFID2_exit(&mfrc2);

	change_lcd_message(START_MESSAGE);

	// Every LCD message changes the SSP configuration, must configure it for
	// the RFID again
	//PCD_Init(mfrc1, LPC_SSP1);

	while (1) {

		updateClockRTC();
/*
		// Look for new cards in RFID1
		if (PICC_IsNewCardPresent(mfrc1)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc1)) {
				userTapIn();
				saveTapInData();
			}
		}
*/

		// Look for new cards in RFID2
		if (PICC_IsNewCardPresent(mfrc2)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc2)) {
				userTapOut();

			}
		}


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

	// show card UID
	DEBUGOUT("Card uid: ");
	for (uint8_t i = 0; i < mfrc1->uid.size; i++) {
		DEBUGOUT(" %X", mfrc1->uid.uidByte[i]);
	}
	DEBUGOUT("\n\r");

	/*
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
	*/
	set_lcd_balance(2340);
	change_lcd_message(USTATUS_AUTHORIZED);
	delay_ms(2000);
	change_lcd_message(START_MESSAGE);
}

void userTapOut() {

	// show card UID
	DEBUGOUT("Card uid: ");
	for (uint8_t i = 0; i < mfrc2->uid.size; i++) {
		DEBUGOUT(" %X", mfrc2->uid.uidByte[i]);
	}
	DEBUGOUT("\n\r");

	// Convert the uid bytes to an integer, byte[0] is the MSB
	last_user_ID =
		(int)mfrc2->uid.uidByte[3] |
		(int)mfrc2->uid.uidByte[2] << 8 |
		(int)mfrc2->uid.uidByte[1] << 16 |
		(int)mfrc2->uid.uidByte[0] << 24;

	if (tapin_flag == 0) {
		if (last_user_ID == userID_1) {
			set_lcd_balance(2340);
		} else if (last_user_ID == userID_2) {
			set_lcd_balance(1200);
		} else {
			set_lcd_balance(9970);
		}

		change_lcd_message(USTATUS_AUTHORIZED);
		tapin_flag = 1;
	} else {
		// user is leaving
		if (last_user_ID == userID_1) {
			set_lcd_balance(1970);
		} else if (last_user_ID == userID_2) {
			set_lcd_balance(830);
		} else {
			set_lcd_balance(9600);
		}

		change_lcd_message(USTATUS_TAP_OUT);
		saveTapInData();
		tapin_flag = 0;
	}

	delay_ms(2000);
	change_lcd_message(START_MESSAGE);
}

void saveTapInData() {

	userInfo.userId = 2092088836;
	userInfo.vehicleId = 18456;
	userInfo.fare = 370;
	userInfo.balance = 1970;
	userInfo.distance = 4;
	userInfo.inOdometerMeasure = 76432;
	userInfo.inTimestamp = FullTime;
	userInfo.inLatitude = -25.443827;
	userInfo.inLongitude = -49.268412;
	userInfo.outOdometerMeasure = 76436;
	userInfo.outTimestamp = FullTime;
	userInfo.outLatitude = -25.443827;
	userInfo.outLongitude = -49.268412;

	generateSmartFareJSON(&userInfo, jsonString);
}
/**********************************
 *  Some util functions
 **********************************/

/**
 * Search for a given userID, returns an index to it if found, -1 otherwise
 * @param  userID the iD to search for
 * @return        the index of the user in the usersBuffer
 */
int getUserByID(unsigned int userId) {

	int i;

	for (i = 0; i < USER_BUFFER_SIZE; i++) {
		if (userId == usersBuffer[i].userId) {
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
void addNewUser(unsigned int userId) {
	UserInfo_T new_user;
	new_user.userId = userId;

	// add user to usersBuffer
	usersBuffer[usersBufferIndex] = new_user;
	usersBufferIndex++;
	if (usersBufferIndex > USER_BUFFER_SIZE - 1) {
		// save buffer in other safe place
		// overwrite buffer values
		usersBufferIndex = 0;
	}
}

/**
 * [removeUser  description]
 * @param userId [description]
 */
void removeUser (unsigned int userId){

	//TODO
}
