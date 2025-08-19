#ifndef __USART_H
#define __USART_H
#include "stm32f10x.h"
#include <stdio.h>

#define USART_BAUDRATE       		115200

#define USART1_TX_GPIO_PORT       	GPIOA
#define USART1_TX_GPIO_PIN        	GPIO_Pin_9
#define USART1_RX_GPIO_PORT       	GPIOA
#define USART1_RX_GPIO_PIN        	GPIO_Pin_10

#define USART2_TX_GPIO_PORT       	GPIOA
#define USART2_TX_GPIO_PIN        	GPIO_Pin_2
#define USART2_RX_GPIO_PORT       	GPIOA
#define USART2_RX_GPIO_PIN        	GPIO_Pin_3

#define USART1_IRQ 					USART1_IRQn
#define USART1_IRQHandler     	 	USART1_IRQHandler

#define USART2_IRQ 					USART2_IRQn
#define USART2_IRQHandler     	 	USART2_IRQHandler

void NVIC_Config(void);
void USART_Config(void);

void USART_SendByte(USART_TypeDef* pUSARTx, uint8_t data);
void USART_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t data);
void USART_SendArray(USART_TypeDef* pUSARTx, uint8_t* Array, uint8_t num);
void USART_SendStr(USART_TypeDef* pUSARTx, uint8_t* str);

#endif  /* __USART_H */
