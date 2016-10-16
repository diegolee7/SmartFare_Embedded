/*
 * rfid_manager.c
 *
 *  Created on: 15 de out de 2016
 *      Author: diegolee7
 */

#include "rfid_utils.h"

/****************************************
 * Private variables
 ****************************************/

// auxBuffer used to store the read end write data, each block have 16 bytes,
// auxBuffer must have 18 slots, see MIFARE_Read()
static uint8_t auxBuffer[18];
static uint8_t size = sizeof(auxBuffer);

// return status from MFRC522 functions
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
static int readCardBlock(MFRC522Ptr_t mfrc522, uint8_t sector,
						 uint8_t blockAddr) {

	MIFARE_Key key;
	int i;
	// using FFFFFFFFFFFFh which is the default at chip delivery from the
	// factory
	for (i = 0; i < 6; i++) {
		key.keyuint8_t[i] = 0xFF;
	}

	// Authenticate using key A
	status = (StatusCode)PCD_Authenticate(mfrc522, PICC_CMD_MF_AUTH_KEY_A,
										  blockAddr, &key, &(mfrc522->uid));
	if (status != STATUS_OK) {
		DEBUGOUT("PCD_Authenticate() failed: ");
		DEBUGOUT(GetStatusCodeName(status));
		return -1;
	}

	// Read data from the block
	status = (StatusCode)MIFARE_Read(mfrc522, blockAddr, auxBuffer, &size);
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
static int writeCardBlock(MFRC522Ptr_t mfrc522, uint8_t sector,
						  uint8_t blockAddr) {

	MIFARE_Key key;
	int i;
	// using FFFFFFFFFFFF which is the default at chip delivery from the factory
	for (i = 0; i < 6; i++) {
		key.keyuint8_t[i] = 0xFF;
	}

	// Authenticate using key A
	status = (StatusCode)PCD_Authenticate(mfrc522, PICC_CMD_MF_AUTH_KEY_A,
										  blockAddr, &key, &(mfrc522->uid));
	if (status != STATUS_OK) {
		DEBUGOUT("PCD_Authenticate() failed: ");
		DEBUGOUT(GetStatusCodeName(status));
		return -1;
	}

	// Write data from the block, always write 16 bytes
	status = (StatusCode)MIFARE_Write(mfrc522, blockAddr, auxBuffer, 16);
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

void setupRFID1_entrance(MFRC522Ptr_t* mfrc1) {
	*mfrc1 = MFRC522_Init();
	// Define the pins to use as CS(SS or SSEL) and RST
	Chip_SCU_PinMuxSet(0x1, 12,
					   (SCU_PINIO_FAST | SCU_MODE_FUNC0)); // Set as GPIO
	Chip_SCU_PinMuxSet(0x1, 10,
					   (SCU_PINIO_FAST | SCU_MODE_FUNC0)); // Set as GPIO
	// GPIO1[12]= P2_12
	(*mfrc1)->_chipSelectPin.port = 1;
	(*mfrc1)->_chipSelectPin.pin = 12;
	// GPIO1[10]= P2_9
	(*mfrc1)->_resetPowerDownPin.port = 1;
	(*mfrc1)->_resetPowerDownPin.pin = 10;
	PCD_Init(*mfrc1, LPC_SSP1);
	DEBUGOUT("Reader 1 ");
	PCD_DumpVersionToSerial(
		*mfrc1); // Show details of PCD - MFRC522 Card Reader details
}

void setupRFID2_exit(MFRC522Ptr_t* mfrc2) {
	// Repeat config for RFID reader 2
	*mfrc2 = MFRC522_Init();
	// Define the pins to use as CS(SS or SSEL) and RST
	Chip_SCU_PinMuxSet(0x1, 0, (SCU_PINIO_FAST | SCU_MODE_FUNC0)); // Set as
																   // GPIO
	Chip_SCU_PinMuxSet(0x5, 02,
					   (SCU_PINIO_FAST | SCU_MODE_FUNC4)); // Set as GPIO
	// GPIO1[0]= P1_07
	(*mfrc2)->_chipSelectPin.port = 1;
	(*mfrc2)->_chipSelectPin.pin = 0;
	// GPIO5[02]= P2_02
	(*mfrc2)->_resetPowerDownPin.port = 5;
	(*mfrc2)->_resetPowerDownPin.pin = 2;
	PCD_Init(*mfrc2, LPC_SSP0);
	DEBUGOUT("Reader 2 ");
	PCD_DumpVersionToSerial(
		*mfrc2); // Show details of PCD - MFRC522 Card Reader details
}

/**
 * Function to read the balance, the balance is stored in the block 4 (sector
 * 1), the first 4 bytes
 * @param  mfrc522 MFRC522 ADT pointer
 * @return         the balance istored in the PICC, -999 for reading erros
 */
int readCardBalance(MFRC522Ptr_t mfrc522) {

	int balance;

	int readStatus = readCardBlock(mfrc522, 1, 4);

	if (readStatus == 0) {
		// convert the balance bytes to an integer, byte[0] is the MSB
		balance = (int)auxBuffer[3] | (int)(auxBuffer[2] << 8) |
				  (int)(auxBuffer[1] << 16) | (int)(auxBuffer[0] << 24);
	} else {
		balance = -999;
	}

	return balance;
}

/**
 * Function to rwrite the balance, the balance is stored in the block 4 (sector
 * 1), the first 4 bytes
 * @param  mfrc522    MFRC522 ADT pointer
 * @param  newBalance the balance to be writte  i  the card
 * @return            0 is no errors
 */
int writeCardBalance(MFRC522Ptr_t mfrc522, int newBalance) {

	// set the 16 bytes auxBuffer, auxBuffer[0] is the MSB
	auxBuffer[0] = newBalance >> 24;
	auxBuffer[1] = newBalance >> 16;
	auxBuffer[2] = newBalance >> 8;
	auxBuffer[3] = newBalance & 0x000000FF;
	int i;
	for (i = 4; i < size; i++) {
		auxBuffer[i] = 0xBB;
	}

	int writeStatus = writeCardBlock(mfrc522, 1, 4);

	return writeStatus;
}