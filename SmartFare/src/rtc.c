#include "rtc.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
static RTC_TIME_T FullTime;

/* Gets and shows the current time and date */
void showTime(RTC_TIME_T *pTime) {
	DEBUGOUT("Time: %.2u:%.2u:%.2u %.2u/%.2u/%.4u\r\n",
			 pTime->time[RTC_TIMETYPE_HOUR],
			 pTime->time[RTC_TIMETYPE_MINUTE],
			 pTime->time[RTC_TIMETYPE_SECOND],
			 pTime->time[RTC_TIMETYPE_MONTH],
			 pTime->time[RTC_TIMETYPE_DAYOFMONTH],
			 pTime->time[RTC_TIMETYPE_YEAR]);
}
/*****************************************************************************
 * Public functions
 ****************************************************************************/

RTC_TIME_T  RTC_getFullTime(){
	return FullTime;
}


/**
 * @brief   RTC interrupt handler
 * @return  Nothing
 */
void RTC_IRQHandler(void) {
	uint32_t sec;

	/* Toggle heart beat LED for each second field change interrupt */
	if (Chip_RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE)) {
		/* Clear pending interrupt */
		Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE);
		RTC_On0 = (bool)!RTC_On0;
		Board_LED_Set(0, RTC_On0);
	}

	/* display timestamp every second in the background */
	sec = Chip_RTC_GetTime(LPC_RTC, RTC_TIMETYPE_SECOND);
	if (!(sec % 1)) {
		oneSecondReachedRTC = true; /* set flag for background */
	}
}

void setupRTC() {

	oneSecondReachedRTC = 0;
	RTC_On0 = RTC_On1 = false;

	Chip_RTC_Init(LPC_RTC);

	set_starting_RTC_time();

	Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

	/* Set the RTC to generate an interrupt on each second */
	Chip_RTC_CntIncrIntConfig(LPC_RTC, RTC_AMR_CIIR_IMSEC, ENABLE);

	/* Clear interrupt pending */
	Chip_RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE | RTC_INT_ALARM);

	/* Enable RTC interrupt in NVIC */
	NVIC_EnableIRQ((IRQn_Type)RTC_IRQn);

	/* Enable RTC (starts increase the tick counter and second counter register)
	 */
	Chip_RTC_Enable(LPC_RTC, ENABLE);

	DEBUGOUT("RTC startted at: ");
	showTime(&FullTime);
}

void set_starting_RTC_time() {
	FullTime.time[RTC_TIMETYPE_SECOND] = 0;
	FullTime.time[RTC_TIMETYPE_MINUTE] = 0;
	FullTime.time[RTC_TIMETYPE_HOUR] = 14;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH] = 15;
	//FullTime.time[RTC_TIMETYPE_DAYOFWEEK] = 5;
	//FullTime.time[RTC_TIMETYPE_DAYOFYEAR] = 279;
	FullTime.time[RTC_TIMETYPE_MONTH] = 10;
	FullTime.time[RTC_TIMETYPE_YEAR] = 2016;
}

void set_current_RTC_time(int seconds,
						  int minutes,
						  int hours,
						  int dayOfMonth,
						  int month,
						  int year) {

	FullTime.time[RTC_TIMETYPE_SECOND] = seconds;
	FullTime.time[RTC_TIMETYPE_MINUTE] = minutes;
	FullTime.time[RTC_TIMETYPE_HOUR] = hours;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH] = dayOfMonth;
	FullTime.time[RTC_TIMETYPE_MONTH] = month;
	FullTime.time[RTC_TIMETYPE_YEAR] = year;
}

void updateClockRTC() {
	if (oneSecondReachedRTC) {
		oneSecondReachedRTC = 0;

		RTC_On1 = (bool)!RTC_On1;
		Board_LED_Set(1, RTC_On1);

		/* read and display time */
		Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
		// showTime(&FullTime);
	}
}
