/*
 * lcd_shield.h
 *
 *  Created on: 1 de set de 2016
 *      Author: luisfg30
 */

#ifndef SRC_LCD_SHIELD_H_
#define SRC_LCD_SHIELD_H_

#include "board.h"
#include "lcd_st7565s.h"
#include "lpc_rom8x8.h"

void board_lcd_init();

/**
 * @Brief show a message for each situation
 * @param a- action code
 *	0 -User already registered
 *	1 -Insufficient balance
 *	2 -OK, enough balance
 *	3 -User check out ok
 *	4 -User check out with negative balance
 */
void change_lcd_message(int message_code);
void write_lcd_text (int line, char* string);
void set_lcd_last_userID(unsigned int userID);
void set_lcd_balance(int balance);

#endif /* SRC_LCD_SHIELD_H_ */
