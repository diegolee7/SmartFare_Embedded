#include "jsonGenerator.h"
#include <string.h>

void generateSmartFareJSON(UserInfo_T *userInfo, char *jsonString) {
	char auxString[20];
	char timestampString[30];
	sprintf(jsonString, "{\"userId\":\"%d\",", userInfo->userId);
	sprintf(auxString, "\"vehicleId\":\"%d\",", userInfo->vehicleId);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"fare\":\"%d\",", userInfo->fare);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"balance\":\"%d\",", userInfo->balance);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"distance\":\"%d\",", userInfo->distance);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"inOdometerMeasure\":\"%d\",",
			userInfo->inOdometerMeasure);
	strcat(jsonString, auxString);
	sprintf(timestampString, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
		userInfo->inTimestamp.time[RTC_TIMETYPE_YEAR],
		userInfo->inTimestamp.time[RTC_TIMETYPE_MONTH],
		userInfo->inTimestamp.time[RTC_TIMETYPE_DAYOFMONTH],
		userInfo->inTimestamp.time[RTC_TIMETYPE_HOUR],
		userInfo->inTimestamp.time[RTC_TIMETYPE_MINUTE],
		userInfo->inTimestamp.time[RTC_TIMETYPE_SECOND]);
	sprintf(auxString, "\"inTimestamp\":\"%s\",", timestampString);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"inLatitude\":\"%f\",", userInfo->inLatitude);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"inLongitude\":\"%f\",", userInfo->inLongitude);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"outOdometerMeasure\":\"%d\",",
			userInfo->outOdometerMeasure);
	strcat(jsonString, auxString);
	sprintf(timestampString, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
		userInfo->outTimestamp.time[RTC_TIMETYPE_YEAR],
		userInfo->outTimestamp.time[RTC_TIMETYPE_MONTH],
		userInfo->outTimestamp.time[RTC_TIMETYPE_DAYOFMONTH],
		userInfo->outTimestamp.time[RTC_TIMETYPE_HOUR],
		userInfo->outTimestamp.time[RTC_TIMETYPE_MINUTE],
		userInfo->outTimestamp.time[RTC_TIMETYPE_SECOND]);
	sprintf(auxString, "\"ouTimestamp\":\"%s\",", timestampString);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"outLatitude\":\"%f\",", userInfo->outLatitude);
	strcat(jsonString, auxString);
	sprintf(auxString, "\"outLongitude\":\"%f\"}", userInfo->outLongitude);
	strcat(jsonString, auxString);

	DEBUGOUT("\nJSON String size: %d",strlen(jsonString));
	DEBUGOUT("\nJSON String generated:\n");
	DEBUGOUT("%s",jsonString);
}

