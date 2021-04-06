#include "DynamicIndication.h"



DynamicIndication::DynamicIndication () {
}


DynamicIndication::~DynamicIndication () {
}


void DynamicIndication::setSign(uint8_t  index, GPIO_TypeDef *port, uint16_t pin) {
	if (index <= signsArraySize)
	{
		signsArray[index].pin = pin;
		signsArray[index].port = port;
		GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = signsArray[index].pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(signsArray[index].port, &GPIO_InitStructure);	
		GPIO_WriteBit(signsArray[index].port, signsArray[index].pin, Bit_RESET);
		currentSignsNumber++;
		start = false;
	}
}


void DynamicIndication::setNumbersPin(BCDDecoder *decoder) {
	mDecoder = decoder;
	currentSignsNumber = 0;
	internalTimer = 0;
}


void DynamicIndication::process() {
	

	if (start)
	{
		if (internalTimer == 0)
		{
			currentSigns = 0;
			clearSigns();
			GPIO_WriteBit(signsArray[currentSigns].port, signsArray[currentSigns].pin, Bit_SET);
			mDecoder->setValue(signsArray[currentSigns].number);
			currentSigns++;
		}
		if (internalTimer == 4)
		{
			clearSigns();
			GPIO_WriteBit(signsArray[currentSigns].port, signsArray[currentSigns].pin, Bit_SET);
			mDecoder->setValue(signsArray[currentSigns].number);
			currentSigns++;
		}
		if (internalTimer == 8) {
			clearSigns();
			GPIO_WriteBit(signsArray[currentSigns].port, signsArray[currentSigns].pin, Bit_SET);
			mDecoder->setValue(signsArray[currentSigns].number);
			currentSigns++;
		}
		if (internalTimer == 12) {
			clearSigns();
			GPIO_WriteBit(signsArray[currentSigns].port, signsArray[currentSigns].pin, Bit_SET);
			mDecoder->setValue(signsArray[currentSigns].number);
			currentSigns++;
		}
		internalTimer++;
		if (internalTimer >= 16) {
			internalTimer = 0;
			currentSigns = 0;
		}

	}
}


void DynamicIndication::setNumber(uint8_t  index, uint8_t number) {
	if (index <= signsArraySize) {
		signsArray[index].number = number;
	}
}


void DynamicIndication::clearSigns() {
	for (uint8_t i = 0; i < 4; i++) {
		GPIO_WriteBit(signsArray[i].port, signsArray[i].pin, Bit_RESET);
	}
	int i;
	for (i = 0; i < 4000; i++)
		asm("nop");
}


void DynamicIndication::startIndication(bool state) {
	start = state;
	clearSigns();
}
