/*
 * SmartFareData.h
 *
 *  Created on: 26 de set de 2016
 *      Author: luisfg30
 */

#ifndef INC_SMARTFAREDATA_H_
#define INC_SMARTFAREDATA_H_

#define  USER_BUFFER_SIZE  10

// balance in cents
static const int min_balance = 300;

//variables that are updated outside (when user taps the card)
volatile unsigned int user_ID;
volatile int lcd_balance_;
volatile int travel_fare;

typedef enum __UserStatus{

    START_MESSAGE,
	USTATUS_UNAUTHORIZED,		//user is already onboard
	USTATUS_INSUF_BALANCE,		//user have insufficient balance
	USTATUS_AUTHORIZED,			//user have enough balance
	USTATUS_TAP_OUT,			//user leaves the vehicle, new_balance > minimum_balance
	USTATUS_TAP_OUT_LOW_BALANCE
}UserStatus;

typedef struct {
	unsigned int userID;		//the PICC used stores a 4 byte user ID
	int balance; 	   		   // the user balance in cents, may be negative

}UserInfo_T;

#endif /* INC_SMARTFAREDATA_H_ */
