#include "BCDDecoder.h"



BCDDecoder::BCDDecoder() {
}


BCDDecoder::~BCDDecoder() {
}


void BCDDecoder::init(GPIO_TypeDef *port, uint16_t Apin, uint16_t Bpin, uint16_t Cpin, uint16_t Dpin) {
	mport = port;
	mApin = Apin;
	mBpin = Bpin;
	mCpin = Cpin;
	mDpin = Dpin;
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = mApin | mBpin | mCpin | mDpin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(mport, &GPIO_InitStructure);	
	//GPIO_WriteBit(mport, mApin | mBpin | mCpin | mDpin, Bit_RESET);
}




void BCDDecoder::setValue(uint8_t value) {
	
	uint16_t pinStatus = 0;

	if ((value & (1 << 0)))
	{
		pinStatus |= mApin;
	}
	if ((value & (1 << 1))) {
		pinStatus |= mBpin;
	}
	if ((value & (1 << 2))) {
		pinStatus |= mCpin;
	}
	if ((value & (1 << 3))) {
		pinStatus |= mDpin;
	}
	GPIO_WriteBit(mport, mApin | mBpin | mCpin | mDpin, Bit_RESET);
	GPIO_WriteBit(mport, pinStatus, Bit_SET);
}


void BCDDecoder::clear() {

}
