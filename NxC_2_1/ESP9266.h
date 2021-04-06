#pragma once
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <string.h>
#include <stdio.h>
#include <misc.h>
#include "Interface.h"
class ESP9266 :
	public Interface
{
public:
	ESP9266 ();
	~ESP9266 ();
	void init(USART_TypeDef *usart, GPIO_TypeDef *port, uint16_t rxPin, uint16_t txPin, uint32_t baudrate);
	void sendCommand(const char *cmd, bool sendEnd = false);
	void sendData(uint8_t *data, uint16_t size);
	bool getIP();
	void processUART();
	void processTimer();
	void clearBuffer();
	void closeCurrentConnection();
	bool isConnected();
	bool setMode(uint8_t mode);
	bool setAP(const char *ssid, const char *pass, const char *ip);
	bool connectNetwork(const char* ssid, const char* password);
	bool connectToServerUDP(const char* host, uint16_t port);
	bool sendUDPpacket(const char* msg, uint16_t size);
	bool sendUDPpacket(const char* msg, const char* size);
	bool setAPip(const char *ip);
	void Reset();
	uint8_t *getData(uint8_t size);
	//uint8_t getData(uint8_t size, uint8_t i);
	
	void SendString(const char *str);
	bool hasIncomeData();
	uint8_t getBufferSize();
	uint8_t *getIncomeData();
	
	enum  espMode_t
	{
	     STAMode = 1, SoftAP = 2    
	};
	bool test();
	char broadcastIP[17];
private:
	
	
	USART_InitTypeDef uart;
	USART_TypeDef *espUart;
	uint8_t buffer[255];
	uint16_t currentByte;
	uint8_t bufferSize = 255;
	bool answerReady;
	uint8_t currentMode;
	bool waitForAnswer(const char *answer1, uint16_t timeout, const char *answer2 = nullptr);
	void endCommand();
	void wait(uint16_t timeout);
	uint16_t internalTimer;
	bool timeout;
	char ipoctet1[4];
	char ipoctet2[4];
	char ipoctet3[4];
	char ipoctet4[4];
	
};

