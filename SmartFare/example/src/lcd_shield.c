/*
 * lcd_shield.c
 *
 *  Created on: 1 de set de 2016
 *      Author: luisfg30
 */

#define FONT_SIZE 8; // pixels

#include "lcd_shield.h"
#include "SmartFareData.h"



//Auxiliary variables
char string[16];


void board_lcd_init(){
	//set up the LCD screen
	Board_LCD_Init(); /* Pin Mux and Clock initialization */
	LCD_Init(); /* Initialize LCD Device and Turn it ON */
	LCD_SetFontColor(1);
	LCD_SetFontBgColor(0); /* Background is OFF */
	LCD_SetFont(&font_rom8x8); /* Use 8x8 Fonts */
}


/**
 * show a message for each situation
 */
void change_lcd_message(int message_code) {

	int i,j;

	//clear LCD
	for(i=0;i<LCD_X_RES;i++){
		for(j=0;j<LCD_Y_RES;j++){
			LCD_SetPixel(i,j,0); //0 is color
		}
	}
	LCD_Refresh(0, 0, LCD_X_RES - 1, LCD_Y_RES - 1);

	sprintf(string, "user_ID: %u", user_ID);
	write_lcd_text(0, string);
	switch (message_code) {

	case USTATUS_UNAUTHORIZED:
		write_lcd_text(1, "Unauthorized");
		break;

	case USTATUS_INSUF_BALANCE:
		write_lcd_text(1, "Saldo insufic.");
		sprintf(string, "saldo: %.2f", balance);
		write_lcd_text(2, string);
		sprintf(string, "saldo_min: %.2f", min_balance);
		write_lcd_text(3, string);
		break;

	case USTATUS_AUTHORIZED:
		write_lcd_text(1, "Authorized");
		sprintf(string, "saldo: %.2f", balance);
		write_lcd_text(2, string);
		write_lcd_text(4, "Boa viagem");
		break;

	case USTATUS_TAP_OUT:
		sprintf(string, "tarifa: %.2f", travel_fare);
		write_lcd_text(1, string);
		sprintf(string, "saldo: %.2f", balance);
		write_lcd_text(2, string);
		break;

	case USTATUS_TAP_OUT_LOW_BALANCE:
		sprintf(string, "tarifa: %.2f", travel_fare);
		write_lcd_text(1, string);
		sprintf(string, "saldo: %.2f", balance);
		write_lcd_text(2, string);
		write_lcd_text(4, "*Cobrado na");
		write_lcd_text(5, " prox. recarga");
		break;
	}
}

//size (128x64)
void write_lcd_text (int line, char* text) {
	
	int line_size = FONT_SIZE;
	line_size += 2;
	int number_lines = LCD_Y_RES/line_size;
	if (line < number_lines) {
		LCD_PutStrXY(0, line *line_size, text);
	} 
	
}

