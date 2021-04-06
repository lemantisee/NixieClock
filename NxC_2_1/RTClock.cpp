#include "RTClock.h"



RTClock::RTClock () {
}


RTClock::~RTClock () {
}


void RTClock::init() {

	PWR_BackupAccessCmd(ENABLE);
	if ((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN) {
		RCC_BackupResetCmd(ENABLE);
		RCC_BackupResetCmd(DISABLE);
		RCC_LSEConfig(RCC_LSE_ON);
		while ((RCC->BDCR & RCC_BDCR_LSERDY) != RCC_BDCR_LSERDY) {}
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RTC_SetPrescaler(0x7FFF);
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForSynchro();
	}
}


void RTClock::setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	hours += mTimezone;
	if (hours > 23) hours -= 24;
	uint32_t gen_seconds = seconds + minutes * 60 + hours * 3600;
	RTC_SetCounter(gen_seconds);
}


RTClock::RTC_DateTimeTypeDef * RTClock::getTime() {
	uint32_t gen_seconds = RTC_GetCounter();
	if (gen_seconds >= 86400)
	{
		gen_seconds %= 86400;
	}
	time.RTC_Seconds = gen_seconds % 60;
	time.RTC_Minutes = (gen_seconds % 3600) / 60;
	time.RTC_Hours = gen_seconds / 3600;
	return &time;
}


void RTClock::setTimeZone(uint8_t timezone) {
	mTimezone = timezone;
}
