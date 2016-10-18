/**
 * Software License Agreement (BSD License)

Copyright (c) 2012, Adafruit Industries
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holders nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

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
BSD license, check license for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#ifndef _ADAFRUIT_PCD8544_H
#define _ADAFRUIT_PCD8544_H


// Adjustments to work with the NXP LPCExpresso4337 board
#include "board.h"
#include "delay.h"
#include <stdlib.h>

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80


// This can be modified to change the clock speed if necessary (like for supporting other hardware).
#define NOKIA5110_BIT_RATE 4000000 // Defined as 4MHz in the original library  

//Display pin setup
// CD = P1_8 = GPIO1[11]
#define NOKIA5110_GPIO_CD_PORT 1
#define NOKIA5110_GPIO_CD_PIN 11
// CS = P2_13 = GPIO1[13]
#define NOKIA5110_GPIO_CS_PORT 1
#define NOKIA5110_GPIO_CS_PIN 13
// RESET = P4_3 = GPIO2[3]
#define NOKIA5110_GPIO_RESET_PORT 2
#define NOKIA5110_GPIO_RESET_PIN  3

#define NOKIA5110_SSP_BUFFER_SIZE 1 // always sends one byte at a time

#define NOKIA5110_LPC_SSP_X LPC_SSP0 // Select SSP0 or SSP1


  void Nokia5110_Init(); //constructor

  // void begin(uint8_t contrast = 40, uint8_t bias = 0x04);
    void begin(uint8_t contrast, uint8_t bias);

  void command(uint8_t c);
  void data(uint8_t c);
  
  void setContrast(uint8_t val);
  void clearDisplay(void);
  void display();
  
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  uint8_t getPixel(int8_t x, int8_t y);

  void spiWrite(uint8_t c);


#endif
