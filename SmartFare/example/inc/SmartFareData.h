/*
 * SmartFareData.h
 *
 *  Created on: 26 de set de 2016
 *      Author: luisfg30
 */

#ifndef INC_SMARTFAREDATA_H_
#define INC_SMARTFAREDATA_H_

#define  USER_BUFFER_SIZE  10

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

// Structure used to store user Data
typedef struct {
	unsigned int userID;		//the PICC used stores a 4 byte user ID
	int balance;				// the user balance in cents, may be negative
	unsigned int travel_fare;	// last travel fare calculated
	float LAT_In;				//Latitude when user get on board
	float LONG_IN;				//Longitude when user get on board
	float LAT_Out;				//Latitude when user get out of the vehicle
	float LONG_Out;				//Longitude when user get out of the vehicle
	unsigned int odometer_In;	//vehicle odometer read when user get on board
	unsigned int odometer_Out;	//vehicle odometer read when user get out of the vehicle
	//timeStamp_In
	//timeStampo_Out
}UserInfo_T;

//Used to show proper massages in the LCD displays
typedef enum __UserStatus{

    START_MESSAGE,
	USTATUS_UNAUTHORIZED,		//user is already on board
	USTATUS_INSUF_BALANCE,		//user have insufficient balance
	USTATUS_AUTHORIZED,			//user have enough balance
	USTATUS_TAP_OUT,			//user get out of the vehicle, new_balance > minimum_balance
	USTATUS_TAP_OUT_LOW_BALANCE
}UserStatus;

//Global variables and default values
static const int min_balance = 300;
volatile float latitude;
volatile float longitude;
volatile unsigned int odometer_Value;

#endif /* INC_SMARTFAREDATA_H_ */
