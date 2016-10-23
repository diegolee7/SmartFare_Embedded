#include "jsonGenerator.h"
#include <string.h>

void generateSmartFareJSON(UserInfo_T *userInfo, char *jsonString) {
	char auxString[20];
	char timestampString[30];
	sprintf(jsonString, "{\n\"userId\":\"%d\",", userInfo->userId);
	sprintf(auxString, "\n\"vehicleId\":\"%d\",", userInfo->vehicleId);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"fare\":\"%.2f\",", (float)userInfo->fare/100);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"balance\":\"%.2f\",", (float)userInfo->balance/100);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"distance\":\"%d\",", userInfo->distance);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"inOdometerMeasure\":\"%d\",",
			userInfo->inOdometerMeasure);
	strcat(jsonString, auxString);
	sprintf(timestampString, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
		userInfo->inTimestamp.time[RTC_TIMETYPE_YEAR],
		userInfo->inTimestamp.time[RTC_TIMETYPE_MONTH],
		userInfo->inTimestamp.time[RTC_TIMETYPE_DAYOFMONTH],
		userInfo->inTimestamp.time[RTC_TIMETYPE_HOUR],
		userInfo->inTimestamp.time[RTC_TIMETYPE_MINUTE],
		userInfo->inTimestamp.time[RTC_TIMETYPE_SECOND]);
	sprintf(auxString, "\n\"inTimestamp\":\"%s\",", timestampString);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"inLatitude\":\"%f\",", userInfo->inLatitude);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"inLongitude\":\"%f\",", userInfo->inLongitude);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"outOdometerMeasure\":\"%d\",",
			userInfo->outOdometerMeasure);
	strcat(jsonString, auxString);
	sprintf(timestampString, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
		userInfo->outTimestamp.time[RTC_TIMETYPE_YEAR],
		userInfo->outTimestamp.time[RTC_TIMETYPE_MONTH],
		userInfo->outTimestamp.time[RTC_TIMETYPE_DAYOFMONTH],
		userInfo->outTimestamp.time[RTC_TIMETYPE_HOUR],
		userInfo->outTimestamp.time[RTC_TIMETYPE_MINUTE],
		userInfo->outTimestamp.time[RTC_TIMETYPE_SECOND]);
	sprintf(auxString, "\n\"ouTimestamp\":\"%s\",", timestampString);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"outLatitude\":\"%f\",", userInfo->outLatitude);
	strcat(jsonString, auxString);
	sprintf(auxString, "\n\"outLongitude\":\"%f\"}", userInfo->outLongitude);
	strcat(jsonString, auxString);

	DEBUGOUT("\nJSON String size: %d",strlen(jsonString));
	DEBUGOUT("\nJSON String generated:\n");
	DEBUGOUT("%s",jsonString);
}

