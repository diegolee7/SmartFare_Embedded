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

#include "Nokia5110.h"

/***********************************************************
 * Some structures to trasnfer data using the SSP interface
 ***********************************************************/
static Chip_SSP_DATA_SETUP_T NOKIA5110_data_Setup;
static uint8_t NOKIA_5110_Tx_Buf[NOKIA5110_SSP_BUFFER_SIZE];
static uint8_t NOKIA_5110_Rx_Buf[NOKIA5110_SSP_BUFFER_SIZE];

/************************************************************
 * Private Functions
 ***********************************************************/

// The Data/Command pin must be valid when the eighth bit is
// sent.  The SSI module has hardware input and output FIFOs
// that are 8 locations deep.  Based on the observation that
// the LCD interface tends to send a few commands and then a
// lot of data, the FIFOs are not used when writing
// commands, and they are used when writing data.  This
// ensures that the Data/Command pin status matches the byte
// that is actually being transmitted.
// The write command operation waits until all data has been
// sent, configures the Data/Command pin for commands, sends
// the command, and then waits for the transmission to
// finish.
// The write data operation waits until there is room in the
// transmit FIFO, configures the Data/Command pin for data,
// and then adds the data to the transmit FIFO.

// This is a helper function that sends an 8-bit message to the LCD.
// inputs: type     COMMAND or DATA
//         message  8-bit code to transmit
// outputs: none
// assumes: SSI0 and port A have already been initialized and enabled
void static lcdwrite(enum typeOfWrite type, char message){
  if(type == COMMAND){
  	 // digitalWrite(_dc, LOW);
  	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN,(bool)false);
  } else{
  	 // digitalWrite(_dc, HIGH);
  	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN,(bool)true);
  }

  	// digitalWrite(_cs, LOW);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)false);

  	// Hardware SPI write data.
	NOKIA_5110_Tx_Buf[0] = message;
	NOKIA5110_data_Setup.rx_cnt = NOKIA5110_data_Setup.tx_cnt = 0;
	Chip_SSP_RWFrames_Blocking(NOKIA5110_LPC_SSP_X, &NOKIA5110_data_Setup);

	// digitalWrite(_cs, HIGH);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,(bool)true);
}

/************************************************************
 * Public Functions
 ***********************************************************/

void Nokia5110_Init(void){

	//initialize data transfer structures
	NOKIA5110_data_Setup.length = NOKIA5110_SSP_BUFFER_SIZE;
	NOKIA5110_data_Setup.rx_data = NOKIA_5110_Rx_Buf;
	NOKIA5110_data_Setup.tx_data = NOKIA_5110_Tx_Buf;

	// Setup hardware SPI.
	// SPI.begin();
	Chip_SSP_Init(NOKIA5110_LPC_SSP_X);
	Chip_SSP_Set_Mode(NOKIA5110_LPC_SSP_X, SSP_MODE_MASTER);
	Chip_SSP_SetFormat(NOKIA5110_LPC_SSP_X, SSP_BITS_8, SSP_FRAMEFORMAT_SPI,SSP_CLOCK_CPHA0_CPOL0);
	Chip_SSP_SetBitRate(NOKIA5110_LPC_SSP_X, NOKIA5110_BIT_RATE);
  	Chip_SSP_Enable(NOKIA5110_LPC_SSP_X);

  	//Configure GPIO pins
  	// CD
  	//Chip_SCU_PinMuxSet(NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN,
			//		   (SCU_MODE_FUNC0));
  	//  CS
  	Chip_SCU_PinMuxSet(NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN,
					   (SCU_MODE_FUNC4));
  	// RESET
  	//Chip_SCU_PinMuxSet(NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN,
			//		   (SCU_MODE_FUNC0));

  // Set common pin outputs.
   	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, NOKIA5110_GPIO_CD_PORT, NOKIA5110_GPIO_CD_PIN);
   	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN);
   	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, NOKIA5110_GPIO_CS_PORT, NOKIA5110_GPIO_CS_PIN);


  // toggle RST
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN,(bool)false);

	delay_ms(500);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, NOKIA5110_GPIO_RESET_PORT, NOKIA5110_GPIO_RESET_PIN,(bool)true);


	lcdwrite(COMMAND, 0x21);              // chip active; horizontal addressing mode (V = 0); use extended instruction set (H = 1)
	                                        // set LCD Vop (contrast), which may require some tweaking:
	lcdwrite(COMMAND, CONTRAST);          // try 0xB1 (for 3.3V red SparkFun), 0xB8 (for 3.3V blue SparkFun), 0xBF if your display is too dark, or 0x80 to 0xFF if experimenting
	lcdwrite(COMMAND, 0x04);              // set temp coefficient
	lcdwrite(COMMAND, 0x14);              // LCD bias mode 1:48: try 0x13 or 0x14

	lcdwrite(COMMAND, 0x20);              // we must send 0x20 before modifying the display control mode
	lcdwrite(COMMAND, 0x0C);              // set display control to normal mode: 0x0D for inverse
}


//********Nokia5110_OutChar*****************
// Print a character to the Nokia 5110 48x84 LCD.  The
// character will be printed at the current cursor position,
// the cursor will automatically be updated, and it will
// wrap to the next row or back to the top if necessary.
// One blank column of pixels will be printed on either side
// of the character for readability.  Since characters are 8
// pixels tall and 5 pixels wide, 12 characters fit per row,
// and there are six rows.
// inputs: data  character to print
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutChar(unsigned char data){
  int i;
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
  for(i=0; i<5; i=i+1){
    lcdwrite(DATA, ASCII[data - 0x20][i]);
  }
  lcdwrite(DATA, 0x00);                 // blank vertical line padding
}

//********Nokia5110_OutString*****************
// Print a string of characters to the Nokia 5110 48x84 LCD.
// The string will automatically wrap, so padding spaces may
// be needed to make the output look optimal.
// inputs: ptr  pointer to NULL-terminated ASCII string
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutString(char *ptr){
  while(*ptr){
    Nokia5110_OutChar((unsigned char)*ptr);
    ptr = ptr + 1;
  }
}

//********Nokia5110_OutUDec*****************
// Output a 16-bit number in unsigned decimal format with a
// fixed size of five right-justified digits of output.
// Inputs: n  16-bit unsigned number
// Outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_OutUDec(unsigned short n){
  if(n < 10){
    Nokia5110_OutString("    ");
    Nokia5110_OutChar(n+'0'); /* n is between 0 and 9 */
  } else if(n<100){
    Nokia5110_OutString("   ");
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  } else if(n<1000){
    Nokia5110_OutString("  ");
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else if(n<10000){
    Nokia5110_OutChar(' ');
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
  else {
    Nokia5110_OutChar(n/10000+'0'); /* ten-thousands digit */
    n = n%10000;
    Nokia5110_OutChar(n/1000+'0'); /* thousands digit */
    n = n%1000;
    Nokia5110_OutChar(n/100+'0'); /* hundreds digit */
    n = n%100;
    Nokia5110_OutChar(n/10+'0'); /* tens digit */
    Nokia5110_OutChar(n%10+'0'); /* ones digit */
  }
}

//********Nokia5110_SetCursor*****************
// Move the cursor to the desired X- and Y-position.  The
// next character will be printed here.  X=0 is the leftmost
// column.  Y=0 is the top row.
// inputs: newX  new X-position of the cursor (0<=newX<=11)
//         newY  new Y-position of the cursor (0<=newY<=5)
// outputs: none
void Nokia5110_SetCursor(unsigned char newX, unsigned char newY){
  if((newX > 11) || (newY > 5)){        // bad input
    return;                             // do nothing
  }
  // multiply newX by 7 because each character is 7 columns wide
  lcdwrite(COMMAND, 0x80|(newX*7));     // setting bit 7 updates X-position
  lcdwrite(COMMAND, 0x40|newY);         // setting bit 6 updates Y-position
}

//********Nokia5110_Clear*****************
// Clear the LCD by writing zeros to the entire screen and
// reset the cursor to (0,0) (top left corner of screen).
// inputs: none
// outputs: none
void Nokia5110_Clear(void){
  int i;
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, 0x00);
  }
  Nokia5110_SetCursor(0, 0);
}

//********Nokia5110_DrawFullImage*****************
// Fill the whole screen by drawing a 48x84 bitmap image.
// inputs: ptr  pointer to 504 byte bitmap
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DrawFullImage(const char *ptr){
  int i;
  Nokia5110_SetCursor(0, 0);
  for(i=0; i<(MAX_X*MAX_Y/8); i=i+1){
    lcdwrite(DATA, ptr[i]);
  }
}
char Screen[SCREENW*SCREENH/8]; // buffer stores the next image to be printed on the screen

//********Nokia5110_PrintBMP*****************
// Bitmaps defined above were created for the LM3S1968 or
// LM3S8962's 4-bit grayscale OLED display.  They also
// still contain their header data and may contain padding
// to preserve 4-byte alignment.  This function takes a
// bitmap in the previously described format and puts its
// image data in the proper location in the buffer so the
// image will appear on the screen after the next call to
//   Nokia5110_DisplayBuffer();
// The interface and operation of this process is modeled
// after RIT128x96x4_BMP(x, y, image);
// inputs: xpos      horizontal position of bottom left corner of image, columns from the left edge
//                     must be less than 84
//                     0 is on the left; 82 is near the right
//         ypos      vertical position of bottom left corner of image, rows from the top edge
//                     must be less than 48
//                     2 is near the top; 47 is at the bottom
//         ptr       pointer to a 16 color BMP image
//         threshold grayscale colors above this number make corresponding pixel 'on'
//                     0 to 14
//                     0 is fine for ships, explosions, projectiles, and bunkers
// outputs: none
void Nokia5110_PrintBMP(unsigned char xpos, unsigned char ypos, const unsigned char *ptr, unsigned char threshold){
  long width = ptr[18], height = ptr[22], i, j;
  unsigned short screenx, screeny;
  unsigned char mask;
  // check for clipping
  if((height <= 0) ||              // bitmap is unexpectedly encoded in top-to-bottom pixel order
     ((width%2) != 0) ||           // must be even number of columns
     ((xpos + width) > SCREENW) || // right side cut off
     (ypos < (height - 1)) ||      // top cut off
     (ypos > SCREENH))           { // bottom cut off
    return;
  }
  if(threshold > 14){
    threshold = 14;             // only full 'on' turns pixel on
  }
  // bitmaps are encoded backwards, so start at the bottom left corner of the image
  screeny = ypos/8;
  screenx = xpos + SCREENW*screeny;
  mask = ypos%8;                // row 0 to 7
  mask = 0x01<<mask;            // now stores a mask 0x01 to 0x80
  j = ptr[10];                  // byte 10 contains the offset where image data can be found
  for(i=1; i<=(width*height/2); i=i+1){
    // the left pixel is in the upper 4 bits
    if(((ptr[j]>>4)&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;
    // the right pixel is in the lower 4 bits
    if((ptr[j]&0xF) > threshold){
      Screen[screenx] |= mask;
    } else{
      Screen[screenx] &= ~mask;
    }
    screenx = screenx + 1;
    j = j + 1;
    if((i%(width/2)) == 0){     // at the end of a row
      if(mask > 0x01){
        mask = mask>>1;
      } else{
        mask = 0x80;
        screeny = screeny - 1;
      }
      screenx = xpos + SCREENW*screeny;
      // bitmaps are 32-bit word aligned
      switch((width/2)%4){      // skip any padding
        case 0: j = j + 0; break;
        case 1: j = j + 3; break;
        case 2: j = j + 2; break;
        case 3: j = j + 1; break;
      }
    }
  }
}
// There is a buffer in RAM that holds one screen
// This routine clears this buffer
void Nokia5110_ClearBuffer(void){int i;
  for(i=0; i<SCREENW*SCREENH/8; i=i+1){
    Screen[i] = 0;              // clear buffer
  }
}

//********Nokia5110_DisplayBuffer*****************
// Fill the whole screen by drawing a 48x84 screen image.
// inputs: none
// outputs: none
// assumes: LCD is in default horizontal addressing mode (V = 0)
void Nokia5110_DisplayBuffer(void){
  Nokia5110_DrawFullImage(Screen);
}

