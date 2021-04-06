#pragma once
#include "ESP9266.h"

#define NTP_TIMESTAMP_DELTA 2208988800ull
class NTPHandle
{
public:
	NTPHandle ();
	~NTPHandle ();
	void init(ESP9266 *wifi);
	void setServer(const char *host, uint16_t port);
	bool getNtpRequest();
	bool getTime();
	uint8_t getHours();
	uint8_t getMinutes();
	uint8_t getSeconds();
private:
	ESP9266 *mWifi;
	char *mHost;
	uint16_t mPort;
	typedef struct
	{
		uint32_t test;
		uint32_t rootDelay;      // 32 bits. Total round trip delay time.
		uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
		uint32_t refId;          // 32 bits. Reference clock identifier.

		uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
		uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

		uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
		uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

		uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
		uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

		uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
		uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

	} ntp_packet;
 
	ntp_packet ntpAnswer;
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
	uint8_t timeZone;
	uint32_t SecondsFromStart;
	inline uint32_t htonl(uint32_t val)
	{
		val = ((((val) >> 24) & 0xff) | (((val) >> 8) & 0xff00) | (((val) << 8) & 0xff0000) | (((val) << 24) & 0xff000000));
		return val;
	}
	void Delay();
	
};

