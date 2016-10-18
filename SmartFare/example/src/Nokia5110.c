/*********************************************************************
This is a library for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

#include "nokia5110.h"
#include "Adafruit_GFX.h"

//Some structures to trasnfer data using the SSP interface
static Chip_SSP_DATA_SETUP_T NOKIA5110_data_Setup;
static uint8_t NOKIA_5110_Tx_Buf[NOKIA5110_SSP_BUFFER_SIZE];
static uint8_t NOKIA_5110_Rx_Buf[NOKIA5110_SSP_BUFFER_SIZE];

//Noki5110 display struct
Adafruit_GFXPtr_t display_nokia;

#ifndef _BVt
  #define _BV(x) (1 << (x))
#endif



// #include <Adafruit_GFX.h>

#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif


// the memory buffer for the LCD
uint8_t pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFF, 0xFC, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8,
0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0xC0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x7F,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x87, 0x8F, 0x9F, 0x9F, 0xFF, 0xFF, 0xFF,
0xC1, 0xC0, 0xE0, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE,
0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0xC0, 0xE0, 0xF1, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x1F, 0x0F, 0x0F, 0x87,
0xE7, 0xFF, 0xFF, 0xFF, 0x1F, 0x1F, 0x3F, 0xF9, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xFD, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xF0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
0x7E, 0x3F, 0x3F, 0x0F, 0x1F, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFC, 0xF0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01,
0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F,
0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x1F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};


// reduces how much is refreshed, which speeds it up!
// originally derived from Steve Evans/JCW's mod but cleaned up and
// optimized
//#define enablePartialUpdate

#ifdef enablePartialUpdate
static uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

void Nokia5110_Init(){
	Adafruit_GFX_Init(LCDWIDTH, LCDHEIGHT) ;
}

static void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax) {
#ifdef enablePartialUpdate
  if (xmin < xUpdateMin) xUpdateMin = xmin;
  if (xmax > xUpdateMax) xUpdateMax = xmax;
  if (ymin < yUpdateMin) yUpdateMin = ymin;
  if (ymax > yUpdateMax) yUpdateMax = ymax;
#endif
}



// the most basic function, set a single pixel
void drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= display_nokia->_width) || (y < 0) || (y >= display_nokia_height))
	return;

  int16_t t;
  switch(display_nokia->rotation){
	case 1:
	  t = x;
	  x = y;
	  y =  LCDHEIGHT - 1 - t;
	  break;
	case 2:
	  x = LCDWIDTH - 1 - x;
	  y = LCDHEIGHT - 1 - y;
	  break;
	case 3:
	  t = x;
	  x = LCDWIDTH - 1 - y;
	  y = t;
	  break;
  }

  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
	return;

  // x is which column
  if (color) 
	pcd8544_buffer[x+ (y/8)*LCDWIDTH] |= _BV(y%8);  
  else
	pcd8544_buffer[x+ (y/8)*LCDWIDTH] &= ~_BV(y%8); 

  updateBoundingBox(x,y,x,y);
}


// the most basic function, get a single pixel
uint8_t getPixel(int8_t x, int8_t y) {
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
	return 0;

  return (pcd8544_buffer[x+ (y/8)*LCDWIDTH] >> (y%8)) & 0x1;  
}


void begin(uint8_t contrast, uint8_t bias) {

	//initialize data transfer structures
	NOKIA5110_data_Setup.length = NOKIA5110_SSP_BUFFER_SIZE;
	NOKIA5110_data_Setup.rx_data = NOKIA_5110_Rx_Buf;
	NOKIA5110_data_Setup.tx_data = NOKIA_5110_Tx_Buf;

	// Setup hardware SPI.
	// SPI.begin();
	Chip_SSP_Init(NOKIA5110_LPC_SSP_X);
	// Chip_SSP_Set_Mode(NOKIA5110_LPC_SSP_X, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(NOKIA5110_LPC_SSP_X, SSP_BITS_8, SSP_FRAMEFORMAT_SPI,SSP_CLOCK_CPHA0_CPOL0);
	Chip_SSP_SetBitRate(NOKIA5110_LPC_SSP_X, NOKIA5110_BIT_RATE);
  	Chip_SSP_Enable(NOKIA5110_LPC_SSP_X);

  // Set common pin outputs.
   	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN);
   	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN);
   	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN);
  

  // toggle RST low to reset
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN,(bool)true);
	delay_ms(500);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN,(bool)false);


  // get into the EXTENDED mode!
  command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );

  // LCD bias select (4 is optimal?)
  command(PCD8544_SETBIAS | bias);

  // set VOP
  if (contrast > 0x7f)
	contrast = 0x7f;

  command( PCD8544_SETVOP | contrast); // Experimentally determined


  // normal mode
  command(PCD8544_FUNCTIONSET);

  // Set display to Normal
  command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

  // initial display line
  // set page address
  // set column address
  // write display data

  // set up a bounding box for screen updates

  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
  // Push out pcd8544_buffer to the Display (will show the AFI logo)
  display();
}


inline void spiWrite(uint8_t d) {

	// Hardware SPI write.
	// SPI.transfer(d);
	NOKIA_5110_Tx_Buf[0] = d;
	NOKIA5110_data_Setup.rx_cnt = NOKIA5110_data_Setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(NOKIA5110_LPC_SSP_X, &NOKIA5110_data_Setup);

}



void command(uint8_t c) {
 	 // digitalWrite(_dc, LOW);
  	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN,(bool)false);

	// digitalWrite(_cs, LOW);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)false);
  	spiWrite(c);
	// digitalWrite(_cs, HIGH);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)true);
}

void data(uint8_t c) {
  	// digitalWrite(_dc, HIGH);
  	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN,(bool)true);
  	
	// digitalWrite(_cs, LOW);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)false);
		spiWrite(c);
	// digitalWrite(_cs, HIGH);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)true);
}

void setContrast(uint8_t val) {
  if (val > 0x7f) {
	val = 0x7f;
  }
  command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION );
  command( PCD8544_SETVOP | val); 
  command(PCD8544_FUNCTIONSET);
  
 }



void display(void) {
  uint8_t col, maxcol, p;
  
  for(p = 0; p < 6; p++) {
#ifdef enablePartialUpdate
	// check if this page is part of update
	if ( yUpdateMin >= ((p+1)*8) ) {
	  continue;   // nope, skip it!
	}
	if (yUpdateMax < p*8) {
	  break;
	}
#endif

	command(PCD8544_SETYADDR | p);


#ifdef enablePartialUpdate
	col = xUpdateMin;
	maxcol = xUpdateMax;
#else
	// start at the beginning of the row
	col = 0;
	maxcol = LCDWIDTH-1;
#endif

	command(PCD8544_SETXADDR | col);

	// digitalWrite(_dc, HIGH);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN,(bool)true);

	  // digitalWrite(_cs, LOW);
  	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)false);	  
	for(; col <= maxcol; col++) {
	  spiWrite(pcd8544_buffer[(LCDWIDTH*p)+col]);
	}

	  // digitalWrite(_cs, HIGH);
  	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)true);
  }

  command(PCD8544_SETYADDR );  // no idea why this is necessary but it is to finish the last byte?
#ifdef enablePartialUpdate
  xUpdateMin = LCDWIDTH - 1;
  xUpdateMax = 0;
  yUpdateMin = LCDHEIGHT-1;
  yUpdateMax = 0;
#endif

}

// clear everything
void clearDisplay(void) {
  memset(pcd8544_buffer, 0, LCDWIDTH*LCDHEIGHT/8);
  updateBoundingBox(0, 0, LCDWIDTH-1, LCDHEIGHT-1);
  display_nokia->cursor_y = display_nokia->cursor_x = 0;
}

/*
// this doesnt touch the buffer, just clears the display RAM - might be handy
void clearDisplay(void) {
  
  uint8_t p, c;
  
  for(p = 0; p < 8; p++) {

	st7565_command(CMD_SET_PAGE | p);
	for(c = 0; c < 129; c++) {
	  //uart_putw_dec(c);
	  //uart_putchar(' ');
	  st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
	  st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
	  st7565_data(0x0);
	}     
	}

}

*/
