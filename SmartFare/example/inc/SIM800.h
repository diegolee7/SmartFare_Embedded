/*
 * SIM800.h
 *
 *  Created on: 4 de out de 2016
 *      Author: luisfg30
 */

/*************************************************************************
* SIM800 GPRS/HTTP Library
* Distributed under GPL v2.0
* Written by Stanley Huang <stanleyhuangyc@gmail.com>
* For more information, please visit http://arduinodev.com
*************************************************************************/


#ifndef INC_SIM800_H_
#define INC_SIM800_H_


//Adjustments to work with the NXP LPCExpresso4337 board
#include "board.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "delay.h"

// Pointers to the UART peripheral, interrupt and handler, defined in the setupUART().
#define SIM800_LPC_UARTX       LPC_USART3
#define SIM800_UARTx_IRQn      USART3_IRQn
#define SIM_800_UARTx_IRQHandler UART3_IRQHandler

/* Transmit and receive ring buffer sizes */
#define SIM800_UART_SRB_SIZE 128	/* Send */
#define SIM800_UART_RRB_SIZE 32	/* Receive */

// change this to the pin connect with SIM800 reset pin
#define SIM800_RESET_PORT	3
#define SIM800_RESET_PIN 	5

/* Transmit and receive ring buffers */
STATIC RINGBUFF_T SIM800_txring, SIM800_rxring;

/* Transmit and receive buffers */
static uint8_t SIM800_rxbuff[SIM800_UART_RRB_SIZE], SIM800_txbuff[SIM800_UART_SRB_SIZE];


// define DEBUG to one serial UART to enable debug information output
//#define DEBUG Serial, DEBUG already defined in board

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

// check if there is available serial data
bool available();
char buffer[256];
uint8_t httpState = HTTP_DISABLED;


/**
 * Configure the UART pins and operation mode
 */
void setUptUART(int baudRate);


//uint 8_t checkbuffer(const char* expected1, const char* expected2 = 0, unsigned int timeout = 2000);
uint8_t checkbuffer(const char* expected1, const char* expected2, unsigned int timeout);
void purgeSerial();
uint8_t m_uint8_tsRecv;
uint32_t m_checkTimer;

// initialize the module
bool init();
// setup network
uint8_t setup(const char* apn);
// get network operator name
bool getOperatorName();
// check for incoming SMS
bool checkSMS();
// get signal quality level (in dB)
int getSignalQuality();
// get GSM location and network time
bool getLocation(GSM_LOCATION* loc);
// initialize HTTP connection
bool httpInit();
// terminate HTTP connection
void httpUninit();
// connect to HTTP server

//Default value if not set in function call, only works in C++
//bool httpConnect(const char* url, const char* args = 0);
bool httpConnect(const char* url, const char* args);
// check if HTTP connection is established
// return 0 for in progress, 1 for success, 2 for error
uint8_t httpIsConnected();
// read data from HTTP connection
void httpRead();
// check if HTTP connection is established
// return 0 for in progress, -1 for error, uint8_ts of http payload on success
int httpIsRead();
// send AT command and check for expected response
//uint8_t sendCommand(const char* cmd, unsigned int timeout = 2000, const char* expected = 0);
uint8_t sendCommand(const char* cmd, unsigned int timeout, const char* expected);
// send AT command and check for two possible responses
//uint8_t sendCommand(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout = 2000);
//Plain C do not allow function overloads (two distinct implementations of the same function name)
uint8_t sendCommand2Expected(const char* cmd, const char* expected1, const char* expected2, unsigned int timeout);
// toggle low-power mode
bool sleep(bool enabled);



#endif /* INC_SIM800_H_ */
