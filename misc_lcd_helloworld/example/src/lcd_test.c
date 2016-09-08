/*
 * lcd_test.c
 *
 *  Created on: 1 de set de 2016
 *      Author: luisfg30
 */

#include "lcd_test.h"

//variables that are updated outside (when user taps the card)
volatile uint32_t user_ID;
volatile float balance;
volatile float travel_fare;

//Auxiliary variables
char string[16];
uint32_t last_user_ID;
float min_balance = 3.0f;

int font_size = 8; //font_size in pixels

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
void action(int a) {


	int i,j;

	//clear LCD
	for(i=0;i<LCD_X_RES;i++){
		for(j=0;j<LCD_Y_RES;j++){
			LCD_SetPixel(i,j,0); //0 is color
		}
	}
	LCD_Refresh(0, 0, LCD_X_RES - 1, LCD_Y_RES - 1);

	sprintf(string, "user_ID: %u", user_ID);
	LCD_PutStrXY(0, 0, string);
	switch (a) {

	case 0:
		LCD_PutStrXY(0, font_size + 1, "Unauthorized");
		break;

	case 1:
		LCD_PutStrXY(0, font_size + 1, "Saldo insufic.");
		sprintf(string, "saldo: %.2f", balance);
		LCD_PutStrXY(0, 2 * font_size + 1, string);
		sprintf(string, "saldo_min: %.2f", min_balance);
		LCD_PutStrXY(0, 3 * font_size + 1, string);
		break;

	case 2:
		LCD_PutStrXY(0, font_size + 1, "Authorized");
		sprintf(string, "saldo: %.2f", balance);
		LCD_PutStrXY(0, 2 * font_size + 1, string);
		LCD_PutStrXY(0, 4 * font_size + 1, "Boa viagem");
		break;

	case 3:
		sprintf(string, "tarifa: %.2f", travel_fare);
		LCD_PutStrXY(0, font_size + 1, string);
		sprintf(string, "saldo: %.2f", balance);
		LCD_PutStrXY(0, 2 * font_size + 1, string);
		break;

	case 4:
		sprintf(string, "tarifa: %.2f", travel_fare);
		LCD_PutStrXY(0, font_size + 1, string);
		sprintf(string, "saldo: %.2f", balance);
		LCD_PutStrXY(0, 2 * font_size + 1, string);
		LCD_PutStrXY(0, 4 * font_size + 1, "*Cobrado na");
		LCD_PutStrXY(0, 5 * font_size + 1, " prox. recarga");
		break;
	}
}

