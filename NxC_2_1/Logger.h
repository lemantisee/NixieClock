#pragma once
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
class Logger {
public:
	static Logger &instance() {
		static Logger singleton;
		return singleton;
	}
	void log(const char *data) {
		while (*data != 0) {
			while (!USART_GetFlagStatus(USART3, USART_FLAG_TXE))
				;
			USART_SendData(USART3, *data);
			++data;
		}
	}
private:
	Logger(){init(); }
	~Logger() {}
	Logger(const Logger&);
	Logger& operator=(const Logger&);
	void init() {
		GPIO_InitTypeDef GPIO_InitStructureUART;
		GPIO_InitStructureUART.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructureUART.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructureUART.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructureUART);
	
		GPIO_InitStructureUART.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructureUART.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructureUART.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructureUART);
	
		USART_InitTypeDef uart;
		uart.USART_BaudRate = 115200;
		uart.USART_WordLength = USART_WordLength_8b;
		uart.USART_StopBits = USART_StopBits_1;
		uart.USART_Parity = USART_Parity_No;
		uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		uart.USART_Mode =  USART_Mode_Tx;
		USART_Init(USART3, &uart);
		USART_Cmd(USART3, ENABLE);
	}
};