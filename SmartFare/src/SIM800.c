/*
* SIM800.c
* NOTE: Please also check the comments in SIM800.h - they provide useful hints
* and background information.
*/

#include "SIM800.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Transmit and receive ring buffers */
STATIC RINGBUFF_T SIM800_txring, SIM800_rxring;
/* Transmit and receive buffers */
static uint8_t SIM800_rxbuff[SIM800_UART_RRB_SIZE],
	SIM800_txbuff[SIM800_UART_SRB_SIZE];

static uint8_t httpState = HTTP_DISABLED;

// Ver como faz o serial na placa
// SoftwareSerial SIM_SERIAL(10, 11); // RX, TX

/**
 * @brief	UART interrupt handler using ring buffers
 * @return	Nothing
 */
void UARTx_IRQHandler(void) {
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(SIM800_LPC_UARTX, &SIM800_rxring, &SIM800_txring);
}

/**
 * See detailed description in SIM800.h
 */
void setupUART(int baudRate) {

	Board_UART_Init(SIM800_LPC_UARTX);

	/* Setup UART for 115.2K8N1 */
	Chip_UART_Init(SIM800_LPC_UARTX);
	Chip_UART_SetBaud(SIM800_LPC_UARTX, baudRate);
	Chip_UART_ConfigData(SIM800_LPC_UARTX,
						 (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(SIM800_LPC_UARTX,
						 (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(SIM800_LPC_UARTX);

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&SIM800_rxring, SIM800_rxbuff, 1, SIM800_UART_RRB_SIZE);
	RingBuffer_Init(&SIM800_txring, SIM800_txbuff, 1, SIM800_UART_SRB_SIZE);

	/* Reset and enable FIFOs, FIFO trigger level 3 (14 chars) */
	Chip_UART_SetupFIFOS(SIM800_LPC_UARTX,
						 (UART_FCR_FIFO_EN | UART_FCR_RX_RS | UART_FCR_TX_RS |
						  UART_FCR_TRG_LEV3));

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(SIM800_LPC_UARTX, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(SIM800_UARTx_IRQn, 1);
	NVIC_EnableIRQ(SIM800_UARTx_IRQn);

	/*Used to get the systick current value, used to calculate the timeouts */
	SysTick_Init();
}

/**
 * See detailed description in SIM800.h
 */
void UART_Print(const char *str) {
	Chip_UART_SendRB(SIM800_LPC_UARTX, &SIM800_txring, str, strlen(str));
}

bool initSIM800() {
	// SIM_SERIAL.begin(57600);
	setupUART(57600);

	//    pinMode(SIM800_RESET_PIN, OUTPUT);
	Chip_SCU_PinMuxSet(SIM800_GPIO_RESET_PORT, SIM800_GPIO_RESET_PIN,
					   (SCU_PINIO_FAST | SCU_MODE_FUNC0));
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, SIM800_GPIO_RESET_PORT,
							  SIM800_GPIO_RESET_PIN);

	//    digitalWrite(SIM800_RESET_PIN, HIGH);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, SIM800_GPIO_RESET_PORT,
						  SIM800_GPIO_RESET_PIN, (bool)true);

	delay_ms(10);
	//    digitalWrite(SIM800_RESET_PIN, LOW);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, SIM800_GPIO_RESET_PORT,
						  SIM800_GPIO_RESET_PIN, (bool)false);

	delay_ms(100);
	//    digitalWrite(SIM800_RESET_PIN, HIGH);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, SIM800_GPIO_RESET_PORT,
						  SIM800_GPIO_RESET_PIN, (bool)true);

	delay_ms(3000);
	// default timeout is 2000, default, char* expected = 0
	if (sendCommand("AT", 2000, 0)) {
		sendCommand("AT+IPR=57600", 2000, 0);
		sendCommand("ATE0", 2000, 0);
		sendCommand("AT+CFUN=1", 10000, 0);
		return true;
	}
	return false;
}

uint8_t setupSIM800() {
	bool success = false;
	for (uint8_t n = 0; n < 30; n++) {
		DEBUGOUT("\nTrying to setup, attempt: %d", n);
		uint8_t ret = sendCommand("AT+CREG?", 2000, 0);

		if (ret) {
			char *p = strstr(bufferSIM800, "0,");
			if (p) {
				char mode = *(p + 2);
				if (mode == '1' || mode == '5') {
					success = true;
					break;
				}
			}
		}
		delay_ms(1000);
		Board_LED_Toggle(0);
	}

	if (!success)
		return 1;

	if (!sendCommand("AT+CGATT?", 2000, 0))
		return 2;

	if (!sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", 2000, 0))
		return 3;

	UART_Print("AT+SAPBR=3,1,\"APN\",\"");
	UART_Print(TIM_APN);
	UART_Print("\"\r\n");
	if (!sendCommand(0, 2000, 0))
		return 4;

	sendCommand("AT+SAPBR=1,1", 10000, 0);
	sendCommand("AT+SAPBR=2,1", 10000, 0);

	sendCommand("AT+CMGF=1", 2000, 0); // sets the SMS mode to text
	sendCommand("AT+CPMS=\"SM\",\"SM\",\"SM\"", 2000, 0); // selects the memory

	if (!success)
		return 5;

	return 0;
}
bool getOperatorName() {
	// display operator name
	if (sendCommand2Expected("AT+COPS?", "OK\r", "ERROR\r", 2000) == 1) {
		char *p = strstr(bufferSIM800, ",\"");
		if (p) {
			p += 2;
			char *s = strchr(p, '\"');
			if (s)
				*s = 0;
			strcpy(bufferSIM800, p);
			return true;
		}
	}
	return false;
}

int getSignalQuality() {
	sendCommand("AT+CSQ", 2000, 0);
	char *p = strstr(bufferSIM800, "CSQ: ");
	if (p) {
		int n = atoi(p + 2);
		if (n == 99 || n == -1)
			return 0;
		return n * 2 - 114;
	} else {
		return 0;
	}
}

bool getLocation(GSM_LOCATION *loc) {
	if (sendCommand("AT+CIPGSMLOC=1,1", 10000, 0))
		do {
			char *p;
			if (!(p = strchr(bufferSIM800, ':')))
				break;
			if (!(p = strchr(p, ',')))
				break;
			loc->lon = atof(++p);
			if (!(p = strchr(p, ',')))
				break;
			loc->lat = atof(++p);
			if (!(p = strchr(p, ',')))
				break;
			loc->year = atoi(++p) - 2000;
			if (!(p = strchr(p, '/')))
				break;
			loc->month = atoi(++p);
			if (!(p = strchr(p, '/')))
				break;
			loc->day = atoi(++p);
			if (!(p = strchr(p, ',')))
				break;
			loc->hour = atoi(++p);
			if (!(p = strchr(p, ':')))
				break;
			loc->minute = atoi(++p);
			if (!(p = strchr(p, ':')))
				break;
			loc->second = atoi(++p);
			return true;
		} while (0);
	return false;
}

void httpUninit() { sendCommand("AT+HTTPTERM", 2000, 0); }

bool httpInit() {
	if (!sendCommand("AT+HTTPINIT", 10000, 0) ||
		!sendCommand("AT+HTTPPARA=\"CID\",1", 5000, 0)) {
		httpState = HTTP_DISABLED;
		return false;
	}
	httpState = HTTP_READY;
	return true;
}
bool httpConnect(const char *url, const char *args) {
	// args=0; //Default value if not set in function call, only works in C++

	// Sets url
	UART_Print("AT+HTTPPARA=\"URL\",\"");
	UART_Print(url);
	if (args) {
		UART_Print("?");
		UART_Print(args);
	}

	UART_Print("\r\n");
	if (sendCommand(0, 2000, 0)) {
		// Starts GET action
		UART_Print("AT+HTTPACTION=0\r\n");
		httpState = HTTP_CONNECTING;
		m_uint8_tsRecv = 0;
		//        m_checkTimer = millis();
		m_checkTimer = SysTick->VAL;
	} else {
		httpState = HTTP_ERROR;
	}
	return false;
}
// check if HTTP connection is established
// return 0 for in progress, 1 for success, 2 for error
uint8_t httpIsConnected() {
	uint8_t ret = checkbuffer("0,200", "0,60", 10000);
	if (ret >= 2) {
		httpState = HTTP_ERROR;
		return -1;
	}
	return ret;
}
void httpRead() {
	UART_Print("AT+HTTPREAD\r\n");
	httpState = HTTP_READING;
	m_uint8_tsRecv = 0;
	m_checkTimer = SysTick->VAL; // millis();
}
// check if HTTP connection is established
// return 0 for in progress, -1 for error, number of http payload uint8_ts on
// success
int httpIsRead() {
	uint8_t ret = checkbuffer("+HTTPREAD: ", "Error", 10000) == 1;
	if (ret == 1) {
		m_uint8_tsRecv = 0;
		// read the rest data
		sendCommand(0, 100, "\r\n");
		int uint8_ts = atoi(bufferSIM800);
		sendCommand(0, 2000, 0);
		uint8_ts = min(uint8_ts, sizeof(bufferSIM800) - 1);
		bufferSIM800[uint8_ts] = 0;
		return uint8_ts;
	} else if (ret >= 2) {
		httpState = HTTP_ERROR;
		return -1;
	}
	return 0;
}

uint8_t sendCommand(const char *command, unsigned int timeout,
					const char *expected) {
	if (command) {
		purgeSerial();
		UART_Print(command);
		UART_Print("\r\n");
		DEBUGOUT("\nSending command: ");
		DEBUGOUT("%s", command);
	}

	uint8_t n = 0;
	uint8_t bytesRead = 0;

	// do {
	while (available()) {
		char c;
		bytesRead = Chip_UART_ReadRB(SIM800_LPC_UARTX, &SIM800_rxring, &c, 1);
		if (n >= sizeof(bufferSIM800) - 1) {
			// buffer full, discard first half
			n = sizeof(bufferSIM800) / 2 - 1;
			memcpy(bufferSIM800, bufferSIM800 + sizeof(bufferSIM800) / 2, n);
		}
		if (bytesRead > 0) {
			bufferSIM800[n++] = c;
			bufferSIM800[n] = 0;
		}
		if (strstr(bufferSIM800, expected ? expected : "OK\r")) {
			DEBUGOUT("\nResponse ok: %s", bufferSIM800);
			return n;
		}
	}
	//} while (SysTick->VAL - t < timeout);//while (millis() - t < timeout);
	DEBUGOUT("\nResponse: %s", bufferSIM800);
	return 0;
}

uint8_t sendCommand2Expected(const char *command, const char *expected1,
							 const char *expected2, unsigned int timeout) {
	if (command) {
		purgeSerial();
		UART_Print(command);
		UART_Print("\r\n");
	}

	uint8_t n = 0;
	uint8_t bytesRead = 0;

	// do {
	while (available()) {
		char c;
		bytesRead = Chip_UART_ReadRB(SIM800_LPC_UARTX, &SIM800_rxring, &c, 1);
		if (n >= sizeof(bufferSIM800) - 1) {
			// buffer full, discard first half
			n = sizeof(bufferSIM800) / 2 - 1;
			memcpy(bufferSIM800, bufferSIM800 + sizeof(bufferSIM800) / 2, n);
		}
		if (bytesRead > 0) {
			bufferSIM800[n++] = c;
			bufferSIM800[n] = 0;
		}
		if (strstr(bufferSIM800, expected1)) {
			DEBUGOUT("\nResponse: %s", bufferSIM800);
			return 1;
		}
		if (strstr(bufferSIM800, expected2)) {
			DEBUGOUT("\nResponse: %s", bufferSIM800);
			return 2;
		}
	}

	//} while (SysTick->VAL - t < timeout);//while (millis() - t < timeout);
	DEBUGOUT("\nResponse: %s", bufferSIM800);
	return 0;
}

uint8_t checkbuffer(const char *expected1, const char *expected2,
					unsigned int timeout) {
	uint8_t n = 0;
	uint8_t bytesRead = 0;
	while (available()) {
		char c;
		bytesRead = Chip_UART_ReadRB(SIM800_LPC_UARTX, &SIM800_rxring, &c, 1);
		if (n >= sizeof(bufferSIM800) - 1) {
			// buffer full, discard first half
			n = sizeof(bufferSIM800) / 2 - 1;
			memcpy(bufferSIM800, bufferSIM800 + sizeof(bufferSIM800) / 2, n);
		}
		if (bytesRead > 0) {
			bufferSIM800[n++] = c;
			bufferSIM800[n] = 0;
		}
		if (strstr(bufferSIM800, expected1)) {
			return 1;
		}
		if (strstr(bufferSIM800, expected2)) {
			return 2;
		}
	}
	return (SysTick->VAL - m_checkTimer < timeout)
			   ? 0
			   : 3; //(millis() - m_checkTimer < timeout) ? 0 : 3;
}

void purgeSerial() {
	uint8_t ch;
	while (Chip_UART_ReadLineStatus(SIM800_LPC_UARTX) & UART_LSR_RDR) {
		Chip_UART_ReadByte(SIM800_LPC_UARTX);
		//also have to remove the data from the ringbuffer
		RingBuffer_Pop(&SIM800_rxring, &ch);
	}
}

bool available() {
	//    return SIM_SERIAL.available();
	if (Chip_UART_ReadLineStatus(SIM800_LPC_UARTX) & UART_LSR_RDR) {
		return false;
	} else {
		return true;
	}
}

bool sleep(bool enabled) {
	return sendCommand(enabled ? "AT+CFUN=0" : "AT+CFUN=1", 2000, 0);
}
