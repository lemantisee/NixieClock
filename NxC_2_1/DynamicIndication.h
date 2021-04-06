#pragma once
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include "BCDDecoder.h"

class DynamicIndication
{
public:
	DynamicIndication ();
	~DynamicIndication ();
	void setSign(uint8_t  index, GPIO_TypeDef *port, uint16_t pin);
	void setNumbersPin(BCDDecoder *decoder);
	void setNumber(uint8_t  index, uint8_t number);
	void process();
	void startIndication(bool state);
	
private:
	struct sign_t
	{
		GPIO_TypeDef *port;
		uint16_t pin;
		uint8_t number;
	};
	void clearSigns();
	
	BCDDecoder *mDecoder;
	sign_t signsArray[6];
	const uint8_t signsArraySize = 6;
	uint8_t currentSignsNumber;
	uint8_t currentSigns;
	volatile uint8_t internalTimer;
	volatile bool start;
	

};

