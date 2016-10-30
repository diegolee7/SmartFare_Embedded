/*
 * rfid_utils.h
 *
 *  Created on: 2016
 *      Author: Diego Gabriel Lee
 *      		Luis Fernando Guerreiro
 */

#ifndef INC_RFID_UTILS_H
#define INC_RFID_UTILS_H

#include "board.h"
#include "MFRC522.h"

/**
 * Setup an MFRC522_T instance and pin configurations. Tailored to our project
 * @param mrfc1 ADT pointer to an MFRC522_T instance
 * Refer to MFRC522.h for detailed information
 */
void setupRFID1_entrance(MFRC522Ptr_t* mrfc1);

/**
 * Setup an MFRC522_T instance and pin configurations. Tailored to our project
 * @param mrfc1 ADT pointer to an MFRC522_T instance
 * Refer to MFRC522.h for detailed information
 */
void setupRFID2_exit(MFRC522Ptr_t* mfrc2);

/**
 * Function to read the balance, the balance is stored in the block 4 (sector
 * 1), the first 4 bytes
 * @param  mfrc522 mfrc522 MFRC522 ADT pointer
 * @return         the balance is stored in the PICC, -999 if reading errors
 */
int readCardBalance(MFRC522Ptr_t mfrc522);

/**
 * Function to write the balance, the balance is stored in the block 4 (sector
 * 1), the first 4 bytes
 * @param  mfrc522    mfrc522 MFRC522 ADT pointer
 * @param  newBalance Desired balance to write to card.
 * @return            0 if no errors
 */
int writeCardBalance(MFRC522Ptr_t mfrc522, int newBalance);

#endif
