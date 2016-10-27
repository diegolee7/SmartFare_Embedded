/**
 *    Copyright 2016
 *    Luis Fernando Guerreiro
 *    Diego Gabriel Lee
 *    Diogo Guilherme Garcia de Freitas
 *
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   Check "LICENSE" file in the root folder of project
 */
/*******************************************************************************
 * LPCXpresso4337 project includes
 ******************************************************************************/
#include "board.h"
#include "chip.h"
#include <cr_section_macros.h>
/*******************************************************************************
 * Custom files and libraries includes
 ******************************************************************************/
#include "lcd_shield.h"
#include "delay.h"
#include "MFRC522.h"
#include "SmartFareData.h"
#include "SIM800.h"
#include "rfid_utils.h"
#include "MFRC522.h"
#include "rtc.h"
#include "jsonGenerator.h"

/*******************************************************************************
 *  Extra functions defined in the main.c file
 ******************************************************************************/
void setupGSM();
void setupRFID();
void userTapIn();
void userTapOut();
int getUserByID(unsigned int userID);
void addNewUser(unsigned int userID);
void removeUser (unsigned int userId);
void setupBluetooth();

/*******************************************************************************
 * Private types/enumerations/variables
 ******************************************************************************/
// temporary variables
int last_balance = 0;
unsigned int last_user_ID;
int usersBufferIndex = 0;

/*******************************************************************************
 * Public types/enumerations/variables
 ******************************************************************************/

volatile float latitude;
volatile float longitude;
volatile unsigned int odometer_Value;

// RFID structs
MFRC522Ptr_t mfrc1;
MFRC522Ptr_t mfrc2;

// buffer to store the active users in the system. On board passengers
static UserInfo_T usersBuffer[USER_BUFFER_SIZE];

//Auxiliary string to format a UserInfo_T struct to a JSON string
static char jsonString[400];


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

	// Initialize shield LCD screen, and SSP interface pins
	board_lcd_init(); //

//	setupGPS();
	setupBluetooth();
//	setupGSM();
	setupRTC();
	setupRFID1_entrance(&mfrc1);
	setupRFID2_exit(&mfrc2);

	change_lcd_message(START_MESSAGE);


	while (1) {

		updateClockRTC();

		// Look for new cards in RFID1
		if (PICC_IsNewCardPresent(mfrc1)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc1)) {
				userTapIn();
			}
		}


		// Look for new cards in RFID2
		if (PICC_IsNewCardPresent(mfrc2)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc2)) {
				userTapOut();
			}
		}

		__WFI();
	}
}

/*******************************************************************************
 *  Peripheral setup functions
 ******************************************************************************/

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
//		DEBUGOUT("\nError Buffer: %s", bufferSIM800);
	}
	DEBUGOUT("\nSetup Successful");
}

void setupBluetooth() {
	//TODO
}

/*******************************************************************************
 *  System routine functions
 ******************************************************************************/

/**
 * Executed every time the card reader detects a user in
 */
void userTapIn() {

	// show card UID
	DEBUGOUT("Card uid: ");
	for (uint8_t i = 0; i < mfrc1->uid.size; i++) {
		DEBUGOUT(" %X", mfrc1->uid.uidByte[i]);
	}
	DEBUGOUT("\n\r");


	// Convert the uid bytes to an integer, byte[0] is the MSB
	last_user_ID =
		(int)mfrc1->uid.uidByte[3] |
		(int)mfrc1->uid.uidByte[2] << 8 |
		(int)mfrc1->uid.uidByte[1] << 16 |
		(int)mfrc1->uid.uidByte[0] << 24;

	// Search for the uID in the usersBuffer
	int userIndex = getUserByID(last_user_ID);
	if (userIndex == -1) {
		// Register user in the buffer
		addNewUser(last_user_ID);
	} else {
		// user is already on board
		change_lcd_message(USTATUS_UNAUTHORIZED);
		PCD_Init(mfrc1, LPC_SSP1);
	}

	// Read the user balance
	last_balance = readCardBalance(mfrc1);
	if (last_balance == (-999)) {
		// Error handling, the card does not have proper balance data inside
	} else {
		// Check for minimum balance
		if (last_balance < MIN_BALANCE) {
			change_lcd_message(USTATUS_INSUF_BALANCE);
			PCD_Init(mfrc1, LPC_SSP1);
		} else {
			set_lcd_last_userID(last_user_ID);
			set_lcd_balance(last_balance);
			change_lcd_message(USTATUS_AUTHORIZED);
			PCD_Init(mfrc1, LPC_SSP1);
		}
	}

	delay_ms(2000);
	change_lcd_message(START_MESSAGE);
}

/**
 * Executed every time the card reader detects a user out
 */
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

	// Search for the uID in the usersBuffer
	int userIndex = getUserByID(last_user_ID);
	if (userIndex == -1) {
		// Show error message, user never taped in
		change_lcd_message(USTATUS_UNAUTHORIZED);
	} else {
		// user is taping out, calculate fare
//		int fare = calculateFare();
		//save data in user struct
//		usersBuffer[userIndex].fare = fare;
//		usersBuffer[userIndex].distance = odometer_Value -
//		usersBuffer[userIndex].inOdometerMeasure;
//		usersBuffer[userIndex].outOdometerMeasure = odometer_Value;
//		usersBuffer[userIndex].outTimestamp = FullTime;
//		usersBuffer[userIndex].outLatitude = latitude;
//		usersBuffer[userIndex].outLongitude = longitude;

		// Calculate fare based on vehicle movement and update user data
//		calculateFare(last_user_ID);
		// Update user balance in the card
		int new_balance = usersBuffer[userIndex].balance - 
		usersBuffer[userIndex].fare;
		writeCardBalance(mfrc2, new_balance );

		//remove user from buffer
		removeUser(last_user_ID);
	}

	delay_ms(2000);
	change_lcd_message(START_MESSAGE);
}


/*******************************************************************************
 *  Some extra functions
 ******************************************************************************/

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
	//create new user struct
	UserInfo_T new_user;
	//assign initial values
	new_user.userId = userId;
//	new_user.vehicleId = VEHICLE_ID;
//	new_user.inOdometerMeasure = odometer_Value;
//	new_user.inTimestamp = FullTime;
//	new_user.inLatitude = latitude;
//	new_user.inLongitude = longitude;

	// add user to userInfoArray
	usersBuffer[usersBufferIndex] = new_user;
	usersBufferIndex++;
	if (usersBufferIndex > USER_BUFFER_SIZE - 1) {
		// save buffer in other safe place
		// overwrite buffer values
		usersBufferIndex = 0;
	}
}

/**
 * Remove a user from the usersBuffer
 * @param userId userID user unique identification number in the system
 */
void removeUser (unsigned int userId){

	//TODO
}

/**
 * Calculate the fare based on the userID data
 * @param  userId user unique identification number in the system
 * @return        calculated fare
 */
int calculateFare(unsigned int userId){

	// Search for the uID in the usersBuffer
	int userIndex = getUserByID(userId);

	//distance in km stored in the userID struct
//	int distance = usersBuffer[userIndex].distance;

	//TODO : define how the fare is charged
	int fare = 370;

	usersBuffer[userIndex].fare = fare;
	return fare;
}
