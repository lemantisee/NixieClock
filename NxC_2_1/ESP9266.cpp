#include "ESP9266.h"
#include "Logger.h"

ESP9266::ESP9266 () {
}


ESP9266::~ESP9266 () {
}


void ESP9266::init(USART_TypeDef *usart, GPIO_TypeDef *port, uint16_t rxPin, uint16_t txPin, uint32_t baudrate) {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = txPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = rxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	espUart = usart;
	uart.USART_BaudRate = baudrate;
	uart.USART_WordLength = USART_WordLength_8b;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(espUart, &uart);
	USART_ITConfig(espUart, USART_IT_RXNE, ENABLE);
	USART_Cmd(espUart, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ClearFlag(espUart, USART_FLAG_RXNE);
	clearBuffer();
	answerReady = false;
	timeout = false;
	currentMode = 0;
	internalTimer = 0;
	Reset();
	//sendCommand("AT+RST"); //echo off
	//endCommand();
	//waitForAnswer("OK");
	//sendCommand("ATE0"); //echo off
	//endCommand();
	//waitForAnswer("OK");
}


void ESP9266::sendCommand(const char *cmd, bool sendEnd) {
	USART_ClearFlag(espUart, USART_FLAG_RXNE);
	USART_ReceiveData(espUart);
	while (*cmd != '\0') {
		while (!USART_GetFlagStatus(espUart, USART_FLAG_TXE));
		USART_SendData(espUart, *cmd);
		++cmd;
	}
	if (sendEnd) endCommand();
}


void ESP9266::processUART() {
	if (USART_GetFlagStatus(espUart, USART_FLAG_RXNE) != RESET) {
		USART_ClearFlag(espUart, USART_FLAG_RXNE);
		uint8_t data = USART_ReceiveData(espUart);
		if (currentByte < bufferSize)
		{
			buffer[currentByte] = data;
			currentByte++;
		}
		else {
			clearBuffer();
		}
			
	}
}



bool ESP9266::setMode(uint8_t mode) {
	currentMode = mode;
	switch (currentMode)
	{
	case STAMode:
		sendCommand("AT+CWMODE=1", true);
		break;
	case SoftAP:
		sendCommand("AT+CWMODE=2", true);
	default:
		break;
	}
	return waitForAnswer("OK", 5000);
}


bool ESP9266::waitForAnswer(const char *answer1, uint16_t timeout, const char *answer2) {
	internalTimer = 0;
	while ((internalTimer < timeout)) {
		if (answer2 != nullptr){
			if ((strstr((char *)buffer, answer1) != nullptr) || (strstr((char *)buffer, answer2) != nullptr)) {
				//Logger::instance().log((char *)buffer);
				return true;
			}
		}
		else{
			if (strstr((char *)buffer, answer1) != nullptr) {
				//Logger::instance().log((char *)buffer);
				return true;
			}
		}
		for (int i = 0; i < 20000; i++) asm("nop");

	}
	//Logger::instance().log("waitForAnswer failed\n");
	clearBuffer();
	return false;
}


void ESP9266::processTimer() {
	internalTimer++;
}


bool ESP9266::connectNetwork(const char* ssid, const char* password) {
	Logger::instance().log("Connecting to wifi network\n");
	sendCommand("AT+CWJAP=");
	sendCommand("\"");
	sendCommand(ssid);
	sendCommand("\"");
	sendCommand(",");
	sendCommand("\"");
	sendCommand(password);
	sendCommand("\"", true);
	bool result = waitForAnswer("OK", 20000);
	if (result) Logger::instance().log("Connected to wifi network\n");
	else Logger::instance().log("Not connected to wifi network\n");
	return result;

	/*
	sendCommand("AT+CIPMUX=0");
	endCommand();
	waitForAnswer("OK");
	sendCommand("AT+CIPMODE=0");
	endCommand();
	waitForAnswer("OK");
	*/
}


void ESP9266::endCommand() {
	while (!USART_GetFlagStatus(espUart, USART_FLAG_TXE));
	USART_SendData(espUart, '\r');
	while (!USART_GetFlagStatus(espUart, USART_FLAG_TXE));
	USART_SendData(espUart, '\n');
	while (!USART_GetFlagStatus(espUart, USART_FLAG_TXE));
	clearBuffer();
}


bool ESP9266::test() {
	USART_ClearFlag(espUart, USART_FLAG_RXNE);
	clearBuffer();
	sendCommand("AT", true);
	Logger::instance().log("Send command AT");
	return waitForAnswer("OK", 1000);
}


bool ESP9266::connectToServerUDP(const char* host, uint16_t port) {
	//AT+CIPSTART="TCP","192.168.0.65",333
	//"UDP", "0", 0, 1025, 2
	char buff[5];
	memset(buff, 0, 5);
	snprintf(buff, 5, "%d", port);
	sendCommand("AT+CIPSTART=\"UDP\",");
	
	sendCommand("\"");
	sendCommand("0");
	sendCommand("\"");
	
	sendCommand(",");
	sendCommand("0");
	sendCommand(",");
//	sendCommand(buff);
//	sendCommand(",");
	sendCommand("1111");
	sendCommand(",");
	sendCommand("2",true);
	return waitForAnswer("OK", 1000, "ALREADY");
}


bool ESP9266::sendUDPpacket(const char* msg, uint16_t size) {

	clearBuffer();
	sendCommand("AT+CIPSEND=48", true);
	waitForAnswer(">", 1000);
	clearBuffer();
	sendData((uint8_t*)msg, size);
	return waitForAnswer("OK", 2000);
}


bool ESP9266::sendUDPpacket(const char* msg, const char* size) {
	sendCommand("AT+CIPSEND=");
	sendCommand(size, true);
	waitForAnswer(">", 1000);
	sendCommand(msg, true);
}


void ESP9266::sendData(uint8_t *data, uint16_t size) {
	for (uint16_t i = 0; i < size; i++)
	{
		while (!USART_GetFlagStatus(espUart, USART_FLAG_TXE));
		USART_SendData(espUart, data[i]);
	}

}


bool ESP9266::getIP() {
	//AT + CIFSR
	memset(broadcastIP, 0, 17);
	memset(ipoctet1, 0, 4);
	memset(ipoctet2, 0, 4);
	memset(ipoctet3, 0, 4);
	memset(ipoctet4, 0, 4);
	sendCommand("AT+CIFSR", true);
	waitForAnswer("OK", 1000);
	char *str1 = strchr((char *)buffer, '"');
	char *str2 = strchr(str1 + 1, '"');
	char iptmp[17];
	strncpy(iptmp, str1 + 1, str2 - str1);
	char *tmp = iptmp;
	strcpy(ipoctet1, strsep(&tmp, "."));
	strcpy(ipoctet2, strsep(&tmp, "."));
	strcpy(ipoctet3, strsep(&tmp, "."));
	strcpy(ipoctet4, strsep(&tmp, "."));
	strcat(broadcastIP, ipoctet1);
	strcat(broadcastIP, ".");
	strcat(broadcastIP, ipoctet2);
	strcat(broadcastIP, ".");
	strcat(broadcastIP, ipoctet3);
	strcat(broadcastIP, ".");
	strcat(broadcastIP, "255");
}

uint8_t * ESP9266::getData(uint8_t size) {
	//clearBuffer();
	char buffSize[3];
	char cmdStr[8] = "+IPD,";
	memset(buffSize, 0, 3);
	sprintf(buffSize, "%i", size);
	strcat(cmdStr, buffSize);
	if (waitForAnswer(cmdStr, 2000)){
		wait(1000);
		char *ptr = strstr((char *)buffer, cmdStr);
		///while (currentByte < (size + 10));
		return ((uint8_t*)ptr + 8);
	}
	else{
		return nullptr;
	}

}

bool ESP9266::setAP(const char *ssid, const char *pass, const char *ip) {
	//AT + CWSAP = "Test2", "habrahabr", 10, 4
	//AT + CWSAP ?
	//sendCommand("AT+CWSAP?");
	//AT + CIPAP_CUR = "192.168.5.1"

	//waitForAnswer("OK", 5000);
	sendCommand("AT+CWSAP=");
	sendCommand("\"");
	sendCommand(ssid);
	sendCommand("\"");
	sendCommand(",");
	sendCommand("\"");
	sendCommand(pass);
	sendCommand("\"");
	sendCommand(",");
	sendCommand("10,3", true);
	waitForAnswer("OK", 5000);
}


void ESP9266::SendString(const char *str) {
	uint8_t strSize = strlen(str);
	char buffSize[3];
	memset(buffSize, 0, 3);
	snprintf(buffSize, 3, "%d", strSize);
	sendCommand("AT+CIPSEND=");
	sendCommand(buffSize, true);
	waitForAnswer(">", 1000);
	Logger::instance().log("Sending data:");
	Logger::instance().log(str);
	Logger::instance().log("\n");
	sendCommand(str);
}


uint8_t ESP9266::getBufferSize() {
	return bufferSize;
}


uint8_t * ESP9266::getIncomeData() {
	char *ptr = strchr((char *)buffer, ':');
	return ((uint8_t*)ptr + 1);
}


bool ESP9266::hasIncomeData() {
	if (strstr((char *)buffer, "IPD") != nullptr) {
		internalTimer = 0;
		while (internalTimer < 500);
		return true;
	}
	return false;
}


void ESP9266::clearBuffer() {
	currentByte = 0;
	memset(buffer, 0, bufferSize);
}


void ESP9266::closeCurrentConnection() {
	Logger::instance().log("Closing current connection\n");
	sendCommand("AT+CIPCLOSE", true);
	waitForAnswer("OK", 1000);
}


bool ESP9266::setAPip(const char *ip) {
	sendCommand("AT+CIPAP=");
	sendCommand("\"");
	sendCommand(ip);
	sendCommand("\"", true);
	waitForAnswer("OK", 5000);
}


void ESP9266::Reset() {
	sendCommand("AT+RST", true);
	endCommand();
	wait(5000);
	//waitForAnswer("OK", 10000);
}


void ESP9266::wait(uint16_t timeout) {
	internalTimer = 0;
	while ((internalTimer < timeout));
}


bool ESP9266::isConnected() {
	sendCommand("AT+CIPSTATUS", true);
	return waitForAnswer("STATUS:2", 3000, "STATUS:3");
}
