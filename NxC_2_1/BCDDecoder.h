#pragma once
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
class BCDDecoder
{
public:
	BCDDecoder();
	~BCDDecoder();
	void init(GPIO_TypeDef *port, uint16_t Apin, uint16_t Bpin, uint16_t Cpin, uint16_t Dpin);
	void setValue(uint8_t value);
private:
	GPIO_TypeDef *mport;
	uint16_t mApin; 
	uint16_t mBpin;
	uint16_t mCpin; 
	uint16_t mDpin;
	inline void Delay() {
		int i;
		for (i = 0; i < 30000; i++)
			asm("nop");
	}
	void clear();
};

