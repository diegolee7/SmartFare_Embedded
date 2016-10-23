/*
 * lcd_shield.h
 *
 *  Created on: 2016
 *      Author: Luis Fernando Guerreiro
 *      		Diego Gabriel Lee
 *
 * 	Licensed under the Apache License, Version 2.0 (the "License");
 * 	Check "LICENSE" file in the root folder of project
 */

#ifndef SRC_LCD_SHIELD_H_
#define SRC_LCD_SHIELD_H_

#include "board.h"
#include "lcd_st7565s.h"
#include "lpc_rom8x8.h"

/**
 * Init the SSP interface and the pins used by the LCD peripheral
 */
void board_lcd_init();

/**
 * Show a predefined message according to mesage_code parameter
 * Refer to __UserStatus ENUM at SmartFareData.h for detailed information
 * @param message_code 
 */
void change_lcd_message(int message_code);

/**
 * Output a string in the LCD screen. The screen is 128 x 64 pixels, 
 * using a 8 pixels wide font we have 8 lines of text with 16 characters 
 * per line.
 * @param line   Desired line (From 0 to 8)
 * @param string Up to 16 characters string message
 */
void write_lcd_text (int line, char* string);

/**
 * Must be called before print_read_user_ID() to update the userID
 * @param userID 
 */
void set_lcd_last_userID(unsigned int userID);

/**
 * Must be called before print_balance(uint8_t line, int balance) the balance
 * @param balance [description]
 */
void set_lcd_balance(int balance);

/**
 * Output formatted userID string in the LCD screen. Ex: "ID do usuario: 12345"
 */
void print_read_user_ID();

/**
 * Output formatted balance string in the LCD screen. Ex: "Saldo: 23,50"
 */
void print_balance(uint8_t line, int balance);

#endif /* SRC_LCD_SHIELD_H_ */
