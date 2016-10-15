#include "rtc.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Gets and shows the current time and date */
void showTime(RTC_TIME_T *pTime) {
	DEBUGOUT("Time: %.2d:%.2d:%.2d %.2d/%.2d/%.4d\r\n",
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

	set_current_RTC_time();

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

}

void set_current_RTC_time() {
	/* Set current time for RTC 2:00:00PM, 2012-10-05 */
	FullTime.time[RTC_TIMETYPE_SECOND] = 0;
	FullTime.time[RTC_TIMETYPE_MINUTE] = 0;
	FullTime.time[RTC_TIMETYPE_HOUR] = 14;
	FullTime.time[RTC_TIMETYPE_DAYOFMONTH] = 5;
	FullTime.time[RTC_TIMETYPE_DAYOFWEEK] = 5;
	FullTime.time[RTC_TIMETYPE_DAYOFYEAR] = 279;
	FullTime.time[RTC_TIMETYPE_MONTH] = 10;
	FullTime.time[RTC_TIMETYPE_YEAR] = 2012;
}
