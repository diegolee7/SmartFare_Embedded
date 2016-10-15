/*
 * jsonGenerator.h
 *
 *  Created on: 14 de out de 2016
 *      Author: diegolee7
 */

#ifndef INC_JSONGENERATOR_H_
#define INC_JSONGENERATOR_H_

#include "board.h"
#include "SmartFareData.h"

void generateSmartFareJSON(UserInfo_T *userInfo, char *jsonString);
void generateTimestampString(RTC_TIME_T *rtc_time_t, char *timestampString);

#endif