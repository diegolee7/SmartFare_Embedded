/*
 * SmartFareData.h
 *
 *  Created on: 26 de set de 2016
 *      Author: luisfg30
 */

// balance in cents
static const int min_balance = 300;

typedef enum __UserStatus{

	USTATUS_UNAUTHORIZED,		//user is already onboard
	USTATUS_INSUF_BALANCE,		//user have insufficient balance
	USTATUS_AUTHORIZED,			//user have enough balance
	USTATUS_TAP_OUT,			//user leaves the vehicle, new_balance > minimum_balance
	USTATUS_TAP_OUT_LOW_BALANCE
}UserStatus;

typedef struct {
	int userID;		//the PICC used stores a 4 byte user ID
	int balance; 	// the user balance in cents.

}UserInfo_T;



#ifndef INC_SMARTFAREDATA_H_
#define INC_SMARTFAREDATA_H_



#endif /* INC_SMARTFAREDATA_H_ */
