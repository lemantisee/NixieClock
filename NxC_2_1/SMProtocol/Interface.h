#pragma once

#include <stdint.h>

class Interface
{
public:
	Interface();
	virtual ~Interface();
	virtual void SendString(const char *str);
	virtual bool hasIncomeData();
	virtual uint8_t getBufferSize();
	virtual uint8_t *getIncomeData();
};

