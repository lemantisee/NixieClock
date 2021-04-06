#pragma once
#include <stdlib.h>
#include <cstring>
#include <string.h>
#include <stdio.h>
#include "Interface.h"
class SMProtocol
{
public:
	SMProtocol();
	~SMProtocol();
	
	void init(Interface *inter);
	bool isCommandReady();
	uint8_t getCommand();
	uint16_t getParametr();
	uint8_t *getStringParametr();
	void SendCommand(const char *str);
	void SendCommand(const char *str, uint32_t parametr);
	void SendCommand(const char *str, const char *parametr);
	void SendCommand(uint8_t cmd, uint32_t parametr);
	void SendCommand(uint8_t cmd, const char *parametr);
	
	
private:
	Interface *mInterface;
	uint16_t parametr;
	char *stringParametrPtr;
	char strstringParametr[100];
};

