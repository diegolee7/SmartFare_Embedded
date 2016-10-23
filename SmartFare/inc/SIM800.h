/*
 * SIM800.h
 *
 *  Created on: 2016
 *      Author: Luis Fernando Guerreiro
 *      		Diego Gabriel Lee
 *
 *
 * 	SIM800 GPRS/HTTP Library C Library adapted from a C++ Arduino Library
 * 	Distributed under GPL v2.0
 *   Written by Stanley Huang <stanleyhuangyc@gmail.com>
 *   For more information, please visit http://arduinodev.com
 */



#ifndef INC_SIM800_H_
#define INC_SIM800_H_

/**********************************************************
 * Adjustments to work with the NXP LPCExpresso4337 board
 *********************************************************/
#include "board.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"

// min redefinition
#define min(a, b) ((a) < (b) ? (a) : (b))

/*******************************************************************************
 * Pointers to the UART peripheral, interrupt and handler, defined in the
 ******************************************************************************/
#define SIM800_LPC_UARTX LPC_USART3
#define SIM800_UARTx_IRQn USART3_IRQn
#define UARTx_IRQHandler UART3_IRQHandler

/*******************************************************************************
 * Transmit and receive ring buffer sizes
 ******************************************************************************/
#define SIM800_UART_SRB_SIZE 128 /* Send */
#define SIM800_UART_RRB_SIZE 64  /* Receive */

/*******************************************************************************
 * Reset Pin configuration
 ******************************************************************************/
// Change this to the pin connect with SIM800 reset pin
// Use P3_5 = GPIO1[15]
#define SIM800_GPIO_RESET_PORT 1
#define SIM800_GPIO_RESET_PIN 15

/*******************************************************************************
 * Types/enumerations/variables
 ******************************************************************************/
#define TIM_APN "timbrasil.br"

#define MAX_ATTEMPTS 30

// define DEBUG to one serial UART to enable debug information output
//#define DEBUG Serial, DEBUG already defined in board.c

static char bufferSIM800[256];
uint8_t m_uint8_tsRecv;
uint32_t m_checkTimer;


typedef enum {
	HTTP_DISABLED = 0,
	HTTP_READY,
	HTTP_CONNECTING,
	HTTP_READING,
	HTTP_ERROR,
} HTTP_STATES;

typedef struct {
	float lat;
	float lon;
	uint8_t year; /* year past 2000, e.g. 15 for 2015 */
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
} GSM_LOCATION;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * check if there is available serial data
 * @return 1 available, 0 not available
 */
bool available();


/**
 * Configure the UART pins and operation mode
 */
void setupUART(int baudRate);

/**
 * Function to use instead of Serial.print
 * Uses predefined UART and ring buffer to output data
 */
void UART_Print(const char *str);

// uint 8_t checkbuffer(const char* expected1, const char* expected2 = 0,
// unsigned int timeout = 2000);
uint8_t checkbuffer(const char *expected1, const char *expected2,
					unsigned int timeout);

/**
 * Clean the RX buffer before reading new data
 */
void purgeSerial();

/**
 * initialize the module
 * @return 1 ok, 0 error
 */
bool initSIM800();
// setup network
uint8_t setupSIM800();
// get network operator name
bool getOperatorName();

// get signal quality level (in dB)
int getSignalQuality();
// get GSM location and network time
bool getLocation(GSM_LOCATION *loc);
// initialize HTTP connection
bool httpInit();
// terminate HTTP connection
void httpUninit();
// connect to HTTP server

// Default value if not set in function call, only works in C++
// bool httpConnect(const char* url, const char* args = 0);
bool httpConnect(const char *url, const char *args);
// check if HTTP connection is established
// return 0 for in progress, 1 for success, 2 for error
uint8_t httpIsConnected();
// read data from HTTP connection
void httpRead();
// check if HTTP connection is established
// return 0 for in progress, -1 for error, uint8_ts of http payload on success
int httpIsRead();
// send AT command and check for expected response
// uint8_t sendCommand(const char* cmd, unsigned int timeout = 2000, const char*
// expected = 0);
uint8_t sendCommand(const char *cmd, unsigned int timeout,
					const char *expected);
// send AT command and check for two possible responses
// uint8_t sendCommand(const char* cmd, const char* expected1, const char*
// expected2, unsigned int timeout = 2000);
// Plain C do not allow function overloads (two distinct implementations of the
// same function name)
uint8_t sendCommand2Expected(const char *cmd, const char *expected1,
							 const char *expected2, unsigned int timeout);
// toggle low-power mode
bool sleep(bool enabled);

#endif /* INC_SIM800_H_ */
