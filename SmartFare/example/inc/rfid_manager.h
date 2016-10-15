/*
 * rfid_manager.h
 *
 *  Created on: 15 de out de 2016
 *      Author: diegolee7
 */

#ifndef INC_RFID_MANAGER_H
#define INC_RFID_MANAGER_H

#include "board.h"
#include "MFRC522.h"

void setupRFID_entrance(MFRC522Ptr_t* mrfc1);
void setupRFID_exit(MFRC522Ptr_t* mrfc2);

#endif