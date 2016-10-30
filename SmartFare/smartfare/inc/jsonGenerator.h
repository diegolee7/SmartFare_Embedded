/*
 * jsonGenerator.h
 *
 *  Created on: 2016
 *      Author: Diego Gabriel Lee
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *	 Check "LICENSE" file in the root folder of project
 */

#ifndef INC_JSONGENERATOR_H_
#define INC_JSONGENERATOR_H_

#include "board.h"
#include "SmartFareData.h"

/**
 * Convert an UserInto_T struct to a JSON formatted string
 * @param userInfo   struct ti be converted
 * @param jsonString output formatted JSON string
 */
void generateSmartFareJSON(UserInfo_T *userInfo, char *jsonString);

#endif
