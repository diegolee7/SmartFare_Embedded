/*
 * rfid_manager.h
 *
 *  Created on: 15 de out de 2016
 *      Author: diegolee7
 */

#ifndef INC_RFID_UTILS_H
#define INC_RFID_UTILS_H

#include "board.h"
#include "MFRC522.h"

void setupRFID1_entrance(MFRC522Ptr_t* mrfc1);
void setupRFID2_exit(MFRC522Ptr_t* mfrc2);
int readCardBalance(MFRC522Ptr_t mfrc522);
int writeCardBalance(MFRC522Ptr_t mfrc522, int newBalance);

#endif