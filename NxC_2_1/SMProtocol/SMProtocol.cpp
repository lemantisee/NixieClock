#include "SMProtocol.h"



SMProtocol::SMProtocol()
{
}


SMProtocol::~SMProtocol()
{
}


void SMProtocol::init(Interface *inter)
{
	mInterface = inter;
	memset(strstringParametr, 0, 100);
}


void SMProtocol::SendCommand(const char *str)
{
	mInterface->SendString(str);
}


void SMProtocol::SendCommand(const char *str, uint32_t parametr)
{
	char str_param[10];
	memset(str_param, 0, 10);
	sprintf(str_param, "%d", parametr);
	char * tmp_str = strcat((char *)str, ":");
	mInterface->SendString(strcat(tmp_str, str_param));
}


void SMProtocol::SendCommand(const char *str, const char *parametr)
{
	char *tmp_str = strcat((char *)str, ":");
	mInterface->SendString(strcat(tmp_str, (char *)parametr));
}


void SMProtocol::SendCommand(uint8_t cmd, uint32_t parametr)
{
	char str_param[10];
	char str_cmd[mInterface->getBufferSize()];
	memset(str_param, 0, 10);
	memset(str_cmd, 0, mInterface->getBufferSize());
	sprintf(str_param, "%d", parametr);
	sprintf(str_cmd, "%d", cmd);
	char * tmp_str = strcat(str_cmd, ":");
	mInterface->SendString(strcat(tmp_str, str_param));
}


void SMProtocol::SendCommand(uint8_t cmd, const char *parametr)
{
	char str_cmd[mInterface->getBufferSize()];
	memset(str_cmd, 0, mInterface->getBufferSize());
	sprintf(str_cmd, "%d", cmd);
	char * tmp_str = strcat(str_cmd, ":");
	mInterface->SendString(strcat(tmp_str, (char *)parametr));
}


bool SMProtocol::isCommandReady()
{
	return mInterface->hasIncomeData();
}


uint8_t SMProtocol::getCommand()
{
	memset(strstringParametr, 0, 100);
	char *tmpPtr = (char *)mInterface->getIncomeData();
	uint8_t command = 0;
	if (tmpPtr != nullptr)
	{
		char *cmd_pos = strchr((char *)mInterface->getIncomeData(), ':');
		if (cmd_pos != nullptr) {
			command = atoi(strsep(&tmpPtr, ":"));
			stringParametrPtr = strsep(&tmpPtr, ":");
			parametr = atoi(stringParametrPtr);
		}
		else {
			command = atoi((char *)mInterface->getIncomeData());
		}
	}


	return command;
}


uint16_t SMProtocol::getParametr()
{
	return parametr;
}


uint8_t * SMProtocol::getStringParametr() {
	strcpy(strstringParametr, stringParametrPtr);
	return (uint8_t *)strstringParametr;
}
