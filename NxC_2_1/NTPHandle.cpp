#include "NTPHandle.h"
#include "Logger.h"

const int mins_in_hour = 60;
const int secs_to_min = 60;
uint8_t ntpRequset[48] = { 0x1B, 0, 0, 0, 0, 0, 0, 0, 0 };

void NTPHandle::Delay() {
	int i;
	for (i = 0; i < 100000; i++)
		asm("nop");
}

NTPHandle::NTPHandle () {
}


NTPHandle::~NTPHandle () {
}


void NTPHandle::init(ESP9266 *wifi) {
	mWifi = wifi;
	memset(&ntpAnswer, 0, 48); 
}


bool NTPHandle::getNtpRequest() {
	mWifi->sendUDPpacket((char *)ntpRequset, 48);
	mWifi->clearBuffer();
	Delay();
	uint8_t *ptr = mWifi->getData(48);
	Delay();
	if(ptr != nullptr)
	{
		memcpy(&ntpAnswer, ptr, sizeof(ntpAnswer));
		SecondsFromStart = (htonl(ntpAnswer.txTm_s) - NTP_TIMESTAMP_DELTA) % 86400;
		mWifi->clearBuffer();
		memset(&ntpAnswer, 0, 48);
		return true;
	}
	else
	{
		return false;
	}
}


bool NTPHandle::getTime() {
	if (SecondsFromStart) {
		Seconds = SecondsFromStart % secs_to_min;
		Minutes = SecondsFromStart / secs_to_min % mins_in_hour;
		Hours = (SecondsFromStart / secs_to_min / mins_in_hour);
		SecondsFromStart = 0;
		return true;
	}
	return false;
}


uint8_t NTPHandle::getHours() {
	return Hours;
}


uint8_t NTPHandle::getMinutes() {
	return Minutes;
}


uint8_t NTPHandle::getSeconds() {
	return Seconds;
}


void NTPHandle::setServer(const char *host, uint16_t port) {
	mHost = (char *)host;
	mPort = port;
	mWifi->connectToServerUDP(mHost, mPort);
}
