#include <stm32f10x_gpio.h>
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include "stm32f10x_pwr.h"
#include "BCDDecoder.h"
#include "DynamicIndication.h"
#include "RTClock.h"
#include "ESP9266.h"
#include "NTPHandle.h"
#include "SMProtocol.h"
#include "Logger.h"

#define F_CPU 		72000000UL
#define TimerTick  	F_CPU/1000-1
volatile uint16_t rtcTubePeriod = 0;
bool updateClock = false;
volatile bool testBlinkStart = false;
volatile uint16_t testTimer = 0;
volatile uint32_t ntpPeriod = 43200000; //12 hours in ms

BCDDecoder dec;
DynamicIndication Indication;
RTClock Clock;
RTClock::RTC_DateTimeTypeDef time;
ESP9266 wifi;
NTPHandle ntpRequest;
SMProtocol protocol;
uint8_t n = 0;
uint32_t Hntp = 0;
uint32_t Mntp = 0;
uint32_t Sntp = 0;


uint8_t testH = 0;
uint8_t testM = 0;
enum vtube_t
{
	MSBHourTube    = 0,
	LSBHourTube    = 1,
	MSBMinutesTube = 2,
	LSBMinutesTube = 3     
};

enum cmd_t
{
	DeviceID     = 100,
	SetWifi      = 101,
	SetNtpServer = 102,
	SetTime      = 103,
	ResetFabric  = 104,
	TestBlink    = 105,
	EnableNTP	 = 106,
	SetTimeZone	 = 107
};

void updateIndication() {
	uint8_t MSB_Hour = Clock.getTime()->RTC_Hours / 10;
	uint8_t LSB_Hour = Clock.getTime()->RTC_Hours % 10;
	uint8_t MSB_Minutes = Clock.getTime()->RTC_Minutes / 10;
	uint8_t LSB_Minutes = Clock.getTime()->RTC_Minutes % 10;
	Indication.setNumber(MSBHourTube, MSB_Hour);
	Indication.setNumber(LSBHourTube, LSB_Hour);
	Indication.setNumber(MSBMinutesTube, MSB_Minutes);
	Indication.setNumber(LSBMinutesTube, LSB_Minutes);
	
}

extern "C" {
	void SysTick_Handler() {
		wifi.processTimer();
		Indication.process();
		++rtcTubePeriod;
		ntpPeriod--;
		if (rtcTubePeriod == 1000) {
			rtcTubePeriod = 0;
			updateIndication();
			updateClock ^= 1;
		}
	}
	void USART1_IRQHandler(void) {
		wifi.processUART();
	}
}

void Delay() {
	int i;
	for (i = 0; i < 6000000; i++)
		asm("nop");
}
void Delay2() {
	int i;
	for (i = 0; i < 100000; i++)
		asm("nop");
}


void update() {

	Indication.setNumber(MSBHourTube, n);
	Indication.setNumber(LSBHourTube, n);
	Indication.setNumber(MSBMinutesTube, n);
	Indication.setNumber(LSBMinutesTube, n);
	n++;
	if (n > 9) {
		n = 0;
	}
}

void switchToAP() {
	Logger::instance().log("Starting AP\n");
	wifi.clearBuffer();
	wifi.setMode(ESP9266::SoftAP);
	wifi.clearBuffer();
	wifi.setAP("NxC2", "NxCPass12", "192.168.4.1");
	wifi.clearBuffer();
	wifi.connectToServerUDP("192.168.4.255", 1112);
	wifi.clearBuffer();
}

void ntpProcess(const char *server){
	ntpRequest.setServer(server, 123);
	for (int i = 0; i < 2; i++) {
		ntpRequest.getNtpRequest();
		Delay();
		if (ntpRequest.getTime()) {
			Logger::instance().log("Ntp correct\n");
			Clock.setTime(ntpRequest.getHours(), ntpRequest.getMinutes(), ntpRequest.getSeconds());
			break;
		}
	}
}

int main() {

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP | RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

	dec.init(GPIOB, GPIO_Pin_6, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_7);
	Logger::instance().log("start main\n");
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);
	
	Indication.setNumbersPin(&dec);
	Indication.setSign(MSBHourTube, GPIOA, GPIO_Pin_6);
	Indication.setSign(LSBHourTube, GPIOA, GPIO_Pin_5);
	Indication.setSign(MSBMinutesTube, GPIOA, GPIO_Pin_4);
	Indication.setSign(LSBMinutesTube, GPIOA, GPIO_Pin_3);
	Clock.init();
	Clock.setTimeZone(3);
	Indication.setNumber(MSBHourTube, 1);
	Indication.setNumber(LSBHourTube, 2);
	Indication.setNumber(MSBMinutesTube, 3);
	Indication.setNumber(LSBMinutesTube, 4);
	Indication.startIndication(true);
	SysTick_Config(TimerTick);
	wifi.init(USART1, GPIOA, GPIO_Pin_10, GPIO_Pin_9, 115200);
	protocol.init(&wifi);
	ntpRequest.init(&wifi);
	wifi.setMode(ESP9266::STAMode);
	Logger::instance().log("Cannot connect ot wifi\n");
	switchToAP();
//	if (!wifi.isConnected()) {
//		Logger::instance().log("Cannot connect ot wifi\n");
//		switchToAP();
//	}
//	else{
//		Logger::instance().log("Connected ot wifi\n");
//		ntpProcess("0.pool.ntp.org");
//	}
	wifi.getIP();

	for (;;) {
		
		if (testBlinkStart) {
			Indication.startIndication(updateClock);
		}
		
		if (!ntpPeriod) {
			ntpPeriod = 43200000; //12 hours in ms
			for (int i = 0; i < 2; i++) {
				ntpRequest.getNtpRequest();
				Delay();
			}
			if (ntpRequest.getTime()) {
				Clock.setTime(ntpRequest.getHours(), ntpRequest.getMinutes(), ntpRequest.getSeconds());
			}
		}
		
		if (protocol.isCommandReady()) {
			char *wStr;
			char *str1;
			char *str2;
			uint8_t cmd = protocol.getCommand();

			uint8_t parametr;
			switch (cmd) {
			case DeviceID:
				Logger::instance().log("Recieve command DeviceID\n");
				wifi.clearBuffer();
				wifi.closeCurrentConnection();
				wifi.clearBuffer();
				wifi.connectToServerUDP(wifi.broadcastIP, 1112);
				wifi.clearBuffer();
				wifi.SendString("NxC_2");
				Delay2();
				wifi.clearBuffer();
				wifi.closeCurrentConnection();
				wifi.clearBuffer();
				wifi.connectToServerUDP("0.pool.ntp.org", 123);
				wifi.clearBuffer();
				break;
			case SetWifi:
				Logger::instance().log("Setting new wifi network\n");
				wStr = (char *)protocol.getStringParametr();
				str1 = strsep(&wStr, ",");
				str2 = strsep(&wStr, ",");
				wifi.closeCurrentConnection();
				wifi.setMode(ESP9266::STAMode);
				if (!wifi.connectNetwork(str1, str2)) {
					switchToAP();
				}
				else {
					wifi.getIP();
					wifi.clearBuffer();
					ntpProcess("0.pool.ntp.org");
				}
				wifi.clearBuffer();
				break;
			case SetTime:
				Logger::instance().log("Setting new time\n");
				wStr = (char *)protocol.getStringParametr();
				str1 = strsep(&wStr, ",");
				str2 = strsep(&wStr, ",");
				wifi.clearBuffer();
				Clock.setTime(atoi(str1), atoi(str2), 0);
				testH = Clock.getTime()->RTC_Hours;
				testM = Clock.getTime()->RTC_Minutes;
				break;
			case SetNtpServer:
				Logger::instance().log("Setting new NTP server\n");
				wStr = (char *)protocol.getStringParametr();
				ntpProcess("0.pool.ntp.org");
				wifi.clearBuffer();
				break;
			case TestBlink:
				parametr = protocol.getParametr();
				wifi.clearBuffer();
				if (parametr) {
					testBlinkStart = true;
				}
				else {
					testBlinkStart = false;
					Indication.startIndication(true);
				}
			  
				break;
			case SetTimeZone:
				Logger::instance().log("Setting new time zone\n");
				{
				wStr = (char *)protocol.getStringParametr();
				wifi.clearBuffer();
				uint8_t timeZone = atoi(wStr);
				Clock.setTimeZone(timeZone);
				uint8_t cH = Clock.getTime()->RTC_Hours;
				uint8_t cM = Clock.getTime()->RTC_Minutes;
				uint8_t cS = Clock.getTime()->RTC_Seconds;
				Clock.setTime(cH, cM, cS);
				}
				break;
			default:
				wifi.clearBuffer();
				break;
			}

		}
	
		

	}
}