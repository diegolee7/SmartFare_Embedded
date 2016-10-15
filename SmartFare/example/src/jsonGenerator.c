#include "jsonGenerator.h"
#include <string.h>

void generateSmartFareJSON(UserInfo_T *userInfo, char *jsonString) {
    char auxString[20];
    char timestampAuxString[30];
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
    generateTimestampString(&userInfo->inTimestamp, timestampAuxString);
    sprintf(auxString, "\"inTimestamp\":\"%s\",", timestampAuxString);
    strcat(jsonString, auxString);
    sprintf(auxString, "\"inLatitude\":\"%d\",", userInfo->inLatitude);
    strcat(jsonString, auxString);
    sprintf(auxString, "\"inLongitude\":\"%d\",", userInfo->inLongitude);
    strcat(jsonString, auxString);
    sprintf(auxString, "\"outOdometerMeasure\":\"%d\",",
            userInfo->outOdometerMeasure);
    strcat(jsonString, auxString);
    generateTimestampString(&userInfo->outTimestamp, timestampAuxString);
    sprintf(auxString, "\"ouTimestamp\":\"%s\",", timestampAuxString);
    strcat(jsonString, auxString);
    sprintf(auxString, "\"outLatitude\":\"%d\",", userInfo->outLatitude);
    strcat(jsonString, auxString);
    sprintf(auxString, "\"outLongitude\":\"%d\"}", userInfo->outLongitude);
    strcat(jsonString, auxString);

    DEBUGOUT("\nJSON String generated:\n");
    DEBUGOUT(jsonString);
}

void generateTimestampString(RTC_TIME_T *rtc_time_t, char *timestampString) {
    sprintf(timestampString, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d",
        rtc_time_t->time[RTC_TIMETYPE_YEAR],
        rtc_time_t->time[RTC_TIMETYPE_MONTH],
        rtc_time_t->time[RTC_TIMETYPE_DAYOFMONTH],
        rtc_time_t->time[RTC_TIMETYPE_HOUR],
        rtc_time_t->time[RTC_TIMETYPE_MINUTE],
        rtc_time_t->time[RTC_TIMETYPE_SECOND]);
}
