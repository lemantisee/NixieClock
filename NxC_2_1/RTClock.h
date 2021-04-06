#pragma once
#include <stdint.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_pwr.h>
class RTClock
{
public:
	RTClock ();
	~RTClock ();
	struct RTC_DateTimeTypeDef
	{
		uint8_t RTC_Hours;
		uint8_t RTC_Minutes;
		uint8_t RTC_Seconds;
	};
	void init();
	void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
	void setTimeZone(uint8_t timezone);
	RTC_DateTimeTypeDef *getTime();

private:
	RTC_DateTimeTypeDef time;
	uint8_t mTimezone = 0;
};

