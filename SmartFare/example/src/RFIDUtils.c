/*
 * RFIDUtils.c
 *
 *  Created on: 26 de set de 2016
 *      Author: luisfg30
 */

/**
 * Some functions to use with the MFRC522 module
 */

#include "MFRC522.h"

/****************************************
 * Private variables
 ****************************************/

//buffer used to store the read end write data, each block have 16 bytes
static uint8_t buffer[16];
static uint8_t size = sizeof(buffer);

//return status from MFRC522 functions
static StatusCode status;

/****************************************
 * Private Functions
 ****************************************/
/**
 * Read 16 bytes from a block inside a sector
 * @param  mfrc522   MFRC522 ADT pointer
 * @param  sector    card sector, 0 to 15
 * @param  blockAddr card block address, 0 to 63
 * @return           0 is no error, -1 is auth errom -2 is read error
 */
static int readCardBlock(MFRC522Ptr_t mfrc522 ,uint8_t sector, uint8_t blockAddr){

	MIFARE_Key key;
	int i;
	   // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
	for ( i = 0; i < 6; i++) {
	    key.keyuint8_t[i] = 0xFF;
	}

    // Authenticate using key A
    status = (StatusCode) PCD_Authenticate(mfrc522,PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522->uid));
    if (status != STATUS_OK) {
        DEBUGOUT("PCD_Authenticate() failed: ");
        DEBUGOUT(GetStatusCodeName(status));
        return -1;
    }

    // Read data from the block
    status = (StatusCode) MIFARE_Read(mfrc522, blockAddr, buffer, &size);
    if (status != STATUS_OK) {
        DEBUGOUT("MIFARE_Read() failed: ");
        DEBUGOUT(GetStatusCodeName(status));
        return -2;
    }

    return 0;
}

/**
 * Write 16 bytes to a block inside a sector
 * @param  mfrc522   MFRC522 ADT pointer
 * @param  sector    card sector, 0 to 15
 * @param  blockAddr card block address, 0 to 63
 * @return           0 is no error, -1 is auth errom -2 is write error
 */
static int writeCardBlock(MFRC522Ptr_t mfrc522 ,uint8_t sector, uint8_t blockAddr){

	MIFARE_Key key;
	int i;
	   // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
	for ( i = 0; i < 6; i++) {
	    key.keyuint8_t[i] = 0xFF;
	}

    // Authenticate using key A
    status = (StatusCode) PCD_Authenticate(mfrc522,PICC_CMD_MF_AUTH_KEY_A, blockAddr, &key, &(mfrc522->uid));
    if (status != STATUS_OK) {
        DEBUGOUT("PCD_Authenticate() failed: ");
        DEBUGOUT(GetStatusCodeName(status));
        return -1;
    }

    // Read data from the block
    status = (StatusCode) MIFARE_Write(mfrc522, blockAddr, buffer, size);
    if (status != STATUS_OK) {
        DEBUGOUT("MIFARE_Write() failed: ");
        DEBUGOUT(GetStatusCodeName(status));
        return -2;
    }

    return 0;
}


/****************************************
 * Public Functions
 ****************************************/

/**
 * Function to read the balance, the balance is stored in the block 4 (sector 1), the first 4 bytes
 * @param  mfrc522 MFRC522 ADT pointer
 * @return         the balance istored in the PICC, -1 for reading erros
 */
float readCardBalance(MFRC522Ptr_t mfrc522 ){

	int balance;

    int readStatus = readCardBlock(mfrc522, 1, 4);

    if(readStatus == 0){
            balance= (int)buffer[0] | (int)(buffer[1]<<8) | (int)(buffer[2]<<16) | (int)(buffer[3]<<24);
    }
    else{
        balance= -1;
    }

    return balance;
}

/**
 * Function to rwrite the balance, the balance is stored in the block 4 (sector 1), the first 4 bytes
 * @param  mfrc522    MFRC522 ADT pointer
 * @param  newBalance the balance to be writte  i  the card
 * @return            0 is no errors
 */
int writeCardBalance(MFRC522Ptr_t mfrc522, float newBalance){

    int writeStatus = writeCardBlock(mfrc522, 1, 4);

    return writeStatus;
}



