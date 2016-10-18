///*
//This is the core graphics library for all our displays, providing a common
//set of graphics primitives (points, lines, circles, etc.).  It needs to be
//paired with a hardware-specific library for each display device we carry
//(to handle the lower-level functions).
//
//Adafruit invests time and resources providing this open source code, please
//support Adafruit & open-source hardware by purchasing products from Adafruit!
//
//Copyright (c) 2013 Adafruit Industries.  All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//
//- Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//- Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//*/
//
//
//#include "Adafruit_GFX.h"
//#include "glcdfont.c"
//
//// Many (but maybe not all) non-AVR board installs define macros
//// for compatibility with existing PROGMEM-reading AVR code.
//// Do our own checks and defines here for good measure...
//
//
//
//#ifndef min
//#define min(a,b) (((a) < (b)) ? (a) : (b))
//#endif
//
//#ifndef abs
//#define abs(x) ((x)<0 ? -(x) : (x))
//#endif
//
//#ifndef _swap_int16_t
//#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
//#endif
//
// //allocate ADT struct
// static struct Adafruit_GFX_T adafruit_struct;
//
//Adafruit_GFXPtr_t Adafruit_GFX_Init(int16_t w, int16_t h){
//
// adafruit_struct.WIDTH = w;
// adafruit_struct.HEIGHT = h;
// adafruit_struct._width    = adafruit_struct.WIDTH;
// adafruit_struct._height   = adafruit_struct.HEIGHT;
// adafruit_struct.rotation  = 0;
// adafruit_struct.cursor_y  = adafruit_struct.cursor_x = 0;
// adafruit_struct.textsize  = 1;
// adafruit_struct.textcolor = adafruit_struct.textbgcolor = 0xFFFF;
// adafruit_struct.wrap      = true;
// adafruit_struct._cp437    = false;
// adafruit_struct.gfxFont   = NULL;
//
// return &adafruit_struct;
//}
//
//// the most basic function, set a single pixel
//void drawPixel(int16_t x, int16_t y, uint16_t color) {
//  if ((x < 0) || (x >= adafruit_struct._width) || (y < 0) || (y >= adafruit_struct._height))
//  return;
//
//  int16_t t;
//  switch(adafruit_struct.rotation){
//  case 1:
//    t = x;
//    x = y;
//    y =  adafruit_struct.HEIGHT - 1 - t;
//    break;
//  case 2:
//    x = adafruit_struct.WIDTH - 1 - x;
//    y = adafruit_struct.HEIGHT - 1 - y;
//    break;
//  case 3:
//    t = x;
//    x = adafruit_struct.WIDTH - 1 - y;
//    y = t;
//    break;
//  }
//
//  if ((x < 0) || (x >= adafruit_struct.WIDTH) || (y < 0) || (y >= adafruit_struct.HEIGHT))
//  return;
//
//  // x is which column
//  if (color)
//  pcd8544_buffer[x+ (y/8)*adafruit_struct.WIDTH] |= _BV(y%8);
//  else
//  pcd8544_buffer[x+ (y/8)*adafruit_struct.WIDTH] &= ~_BV(y%8);
//
//  updateBoundingBox(x,y,x,y);
//}
//
////void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
//// uint16_t color) {
////  // Update in subclasses if desired!
////  for (int16_t i=x; i<x+w; i++) {
////    drawFastVLine(i, y, h, color);
////  }
////}
////
////void drawFastVLine(int16_t x, int16_t y,
//// int16_t h, uint16_t color) {
////  // Update in subclasses if desired!
////  drawLine(x, y, x, y+h-1, color);
////}
//
////void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
//// uint16_t color) {
////  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
////  if (steep) {
////    _swap_int16_t(x0, y0);
////    _swap_int16_t(x1, y1);
////  }
////
////  if (x0 > x1) {
////    _swap_int16_t(x0, x1);
////    _swap_int16_t(y0, y1);
////  }
////
////  int16_t dx, dy;
////  dx = x1 - x0;
////  dy = abs(y1 - y0);
////
////  int16_t err = dx / 2;
////  int16_t ystep;
////
////  if (y0 < y1) {
////    ystep = 1;
////  } else {
////    ystep = -1;
////  }
////
////  for (; x0<=x1; x0++) {
////    if (steep) {
////      drawPixel(y0, x0, color);
////    } else {
////      drawPixel(x0, y0, color);
////    }
////    err -= dy;
////    if (err < 0) {
////      y0 += ystep;
////      err += dx;
////    }
////  }
////}
//
//
//void write(uint8_t c) {
//
//   if(c == '\n') {
//     adafruit_struct.cursor_y += adafruit_struct.textsize*8;
//     adafruit_struct.cursor_x  = 0;
//   } else if(c == '\r') {
//     // skip em
//   } else {
//     if(adafruit_struct.wrap && ((adafruit_struct.cursor_x + adafruit_struct.textsize * 6) >= adafruit_struct._width)) { // Heading off edge?
//       adafruit_struct.cursor_x  = 0;            // Reset x to zero
//       adafruit_struct.cursor_y += adafruit_struct.textsize * 8; // Advance y one line
//     }
//     drawChar(adafruit_struct.cursor_x, adafruit_struct.cursor_y, c, adafruit_struct.textcolor, adafruit_struct.textbgcolor, adafruit_struct.textsize);
//     adafruit_struct.cursor_x += adafruit_struct.textsize * 6;
//   }
//}
//
//// Draw a character
//void drawChar(int16_t x, int16_t y, unsigned char c,
//uint16_t color, uint16_t bg, uint8_t size) {
//
//   if((x >= adafruit_struct._width)            || // Clip right
//      (y >= adafruit_struct._height)           || // Clip bottom
//      ((x + 6 * size - 1) < 0) || // Clip left
//      ((y + 8 * size - 1) < 0))   // Clip top
//     return;
//
//   if(!adafruit_struct._cp437 && (c >= 176)) c++; // Handle 'classic' charset behavior
//
//   for(int8_t i=0; i<6; i++ ) {
//     uint8_t line;
//     if(i < 5) line = font+(c*5)+i;
//     else      line = 0x0;
//     for(int8_t j=0; j<8; j++, line >>= 1) {
//       if(line & 0x1) {
//         if(size == 1) drawPixel(x+i, y+j, color);
//         else          fillRect(x+(i*size), y+(j*size), size, size, color);
//       } else if(bg != color) {
//         if(size == 1) drawPixel(x+i, y+j, bg);
//         else          fillRect(x+i*size, y+j*size, size, size, bg);
//       }
//     }
//   }
//}
//
//void setCursor(int16_t x, int16_t y) {
// adafruit_struct.cursor_x = x;
// adafruit_struct.cursor_y = y;
//}
//
//int16_t getCursorX() {
// return adafruit_struct.cursor_x;
//}
//
//int16_t getCursorY()  {
// return adafruit_struct.cursor_y;
//}
//
//void setTextsize(uint8_t s) {
// adafruit_struct.textsize = (s > 0) ? s : 1;
//}
//
//void settextcolor(uint16_t c) {
// // For 'transparent' background, we'll set the bg
// // to the same as fg instead of using a flag
// adafruit_struct.textcolor = adafruit_struct.textbgcolor = c;
//}
//
//void settextcolorB(uint16_t c, uint16_t b) {
// adafruit_struct.textcolor   = c;
// adafruit_struct.textbgcolor = b;
//}
//
//void setTextwrap(bool w) {
// adafruit_struct.wrap = w;
//}
//
//uint8_t getRotation() {
// return adafruit_struct.rotation;
//}
//
//void setRotation(uint8_t x) {
//	adafruit_struct.rotation = (x & 3);
// switch(adafruit_struct.rotation) {
//  case 0:
//  case 2:
//   adafruit_struct._width  = adafruit_struct.WIDTH;
//   adafruit_struct._height = adafruit_struct.HEIGHT;
//   break;
//  case 1:
//  case 3:
//   adafruit_struct._width  = adafruit_struct.HEIGHT;
//   adafruit_struct._height = adafruit_struct.WIDTH;
//   break;
// }
//}
//
//// Enable (or disable) Code Page 437-compatible charset.
//// There was an error in glcdfont.c for the longest time -- one character
//// (#176, the 'light shade' block) was missing -- this threw off the index
//// of every character that followed it.  But a TON of code has been written
//// with the erroneous character indices.  By default, the library uses the
//// original 'wrong' behavior and old sketches will still work.  Pass 'true'
//// to this function to use correct CP437 character values in your code.
//void cp437(bool x) {
//	adafruit_struct._cp437 = x;
//}
//
//void setFont(const GFXfont *f) {
// if(f) {          // Font struct pointer passed in?
//   if(!adafruit_struct.gfxFont) { // And no current font struct?
//     // Switching from classic to new font behavior.
//     // Move cursor pos down 6 pixels so it's on baseline.
//     adafruit_struct.cursor_y += 6;
//   }
// } else if(adafruit_struct.gfxFont) { // NULL passed.  Current font struct defined?
//   // Switching from new to classic font behavior.
//   // Move cursor pos up 6 pixels so it's at top-left of char.
//   adafruit_struct.cursor_y -= 6;
// }
// adafruit_struct.gfxFont = (GFXfont *)f;
//}
//
//// Pass string and a cursor position, returns UL corner and W,H.
//void getTextBounds(char *str, int16_t x, int16_t y,
//int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
// uint8_t c; // Current character
//
// *x1 = x;
// *y1 = y;
// *w  = *h = 0;
//
// // Default font
//
//   uint16_t lineWidth = 0, maxWidth = 0; // Width of current, all lines
//
//   while((c = *str++)) {
//     if(c != '\n') { // Not a newline
//       if(c != '\r') { // Not a carriage return, is normal char
//         if(adafruit_struct.wrap && ((x + adafruit_struct.textsize * 6) >= adafruit_struct._width)) {
//           x  = 0;            // Reset x to 0
//           y += adafruit_struct.textsize * 8; // Advance y by 1 line
//           if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
//           lineWidth  = adafruit_struct.textsize * 6; // First char on new line
//         } else { // No line adafruit_struct.wrap, just keep incrementing X
//           lineWidth += adafruit_struct.textsize * 6; // Includes interchar x gap
//         }
//       } // Carriage return = do nothing
//     } else { // Newline
//       x  = 0;            // Reset x to 0
//       y += adafruit_struct.textsize * 8; // Advance y by 1 line
//       if(lineWidth > maxWidth) maxWidth = lineWidth; // Save widest line
//       lineWidth = 0;     // Reset lineWidth for new line
//     }
//   }
//   // End of string
//   if(lineWidth) y += adafruit_struct.textsize * 8; // Add height of last (or only) line
//   if(lineWidth > maxWidth) maxWidth = lineWidth; // Is the last or only line the widest?
//   *w = maxWidth - 1;               // Don't include last interchar x gap
//   *h = y - *y1;
//}
//
//
//// Return the size of the display (per current rotation)
//int16_t width() {
// return adafruit_struct._width;
//}
//
//int16_t height()  {
// return adafruit_struct._height;
//}
//
//void invertDisplay(bool i) {
// // Do nothing, must be subclassed if supported by hardware
//}
//
//
//
