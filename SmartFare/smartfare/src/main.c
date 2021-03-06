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
int calculateFare(unsigned int userId);
void removeUser (unsigned int userId);
void setupBluetooth();

/*******************************************************************************
 * Private types/enumerations/variables
 ******************************************************************************/

int last_balance = 0;
unsigned int last_user_ID;
int usersBufferHead = 0;

/*******************************************************************************
 * Public types/enumerations/variables
 ******************************************************************************/

volatile float latitude = 0.00;
volatile float longitude = 0.00;
volatile unsigned int odometer_Value = 12345;

// RFID structs
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
	Board_LED_Set(0, true);

	// Initialize shield LCD screen, and SSP interface pins
	board_lcd_init(); //

//	setupGPS();
	setupBluetooth();
//	setupGSM();
	setupRTC();
	setupRFID(&mfrc2);

	change_lcd_message(START_MESSAGE);


	while (1) {

		updateClockRTC();

		// Look for new cards in RFID2
		if (PICC_IsNewCardPresent(mfrc2)) {
			// Select one of the cards
			if (PICC_ReadCardSerial(mfrc2)) {
				//int status = writeCardBalance(mfrc2, 9999); // used to recharge the card
				userTapIn();
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
	DEBUGOUT("Card uid bytes: ");
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

	DEBUGOUT("  User ID: %d ",last_user_ID);

	// Search for the uID in the usersBuffer
	int userIndex = getUserByID(last_user_ID);

	DEBUGOUT (" userIndex: %d ",userIndex);
	if (userIndex == -1) {
		// New user
	
		// Read the user balance
		last_balance = readCardBalance(mfrc2);

		if (last_balance == (-999)) {
			// Error handling, the card does not have proper balance data inside
		} else {
			// Check for minimum balance
			if (last_balance < MIN_BALANCE) {
				change_lcd_message(USTATUS_INSUF_BALANCE);
			} else {
				set_lcd_last_userID(last_user_ID);
				set_lcd_balance(last_balance);
				change_lcd_message(USTATUS_AUTHORIZED);
				// Register user in the buffer
				addNewUser(last_user_ID);
				DEBUGOUT("usersBufferHead: %d \n", usersBufferHead);
			}
		}
	} else {
		// User is leaving vehicle.

		// Update struct data and remove user from buffer
		removeUser(last_user_ID);
		DEBUGOUT("usersBufferHead: %d \n", usersBufferHead);
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
 * Create an UserInfo_T instance with boarding data and stores it int the
 * usersBuffer
 * @param userID user unique identification number in the system
 */
void addNewUser(unsigned int userId) {
	//create new user struct
	UserInfo_T new_user;

	//assign initial values
	new_user.userId = userId;
	new_user.balance = last_balance;
	new_user.vehicleId = VEHICLE_ID;
	new_user.inOdometerMeasure = odometer_Value;
	new_user.inTimestamp = RTC_getFullTime();
	new_user.inLatitude = latitude;
	new_user.inLongitude = longitude;

	// add user to userInfoArray
	usersBuffer[usersBufferHead] = new_user;
	usersBufferHead ++;

	if (usersBufferHead > USER_BUFFER_SIZE - 1) {
		// save buffer in other safe place
		// overwrite buffer values
		usersBufferHead = 0;
	}
}

/**
 * Remove a user from the usersBuffer after updating the vehicle data and 
 * calculated fare
 * @param userId userID user unique identification number in the system
 */
void removeUser (unsigned int userId){

		int userIndex = getUserByID(userId);
		//save data in user struct
		usersBuffer[userIndex].distance = odometer_Value -
			usersBuffer[userIndex].inOdometerMeasure;
		usersBuffer[userIndex].outOdometerMeasure = odometer_Value;
		usersBuffer[userIndex].outTimestamp = RTC_getFullTime();
		usersBuffer[userIndex].outLatitude = latitude;
		usersBuffer[userIndex].outLongitude = longitude;

		// Calculate fare based on vehicle movement and update user data
		int fare = calculateFare(last_user_ID);

		// Update user balance 
		int new_balance = usersBuffer[userIndex].balance - fare;
		usersBuffer[userIndex].balance = new_balance;

		//update balance in the user card
		writeCardBalance(mfrc2, new_balance);

		//show message in LCD
		
		set_lcd_balance(new_balance);
		set_lcd_travel_fare(fare);
		change_lcd_message(USTATUS_TAP_OUT);

		// Remove user from buffer. Must improve this Data structure and 
		//algorithm latter.
		usersBuffer[userIndex].userId = 0;

		if (userIndex != usersBufferHead - 1){//not in the last position

			//shift all other elements left
			int i; 
			for (i = userIndex ; i < usersBufferHead - 1; i ++){
				usersBuffer[i] = usersBuffer[i + 1];
			}
		}
		usersBufferHead --;
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
