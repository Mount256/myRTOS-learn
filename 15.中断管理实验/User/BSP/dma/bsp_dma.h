#ifndef __DMA_H
#define __DMA_H
#include "stm32f10x_dma.h"

#define SEND_BUFFER_SIZE 	   255
#define RECEIVE_BUFFER_SIZE    255

#define USART1_DMA_TX_Channel  DMA1_Channel4
#define USART1_DMA_RX_Channel  DMA1_Channel5

extern uint8_t  SendBuffer[SEND_BUFFER_SIZE];
extern uint8_t  ReceiveBuffer[RECEIVE_BUFFER_SIZE];
extern uint16_t USART1_RX_Sta;          // 接收状态标志位

void USART_DMA_Config(void);
void USART_DMA_Send(uint8_t *data, uint16_t len);
uint16_t USART_Get_RX_Len(DMA_Channel_TypeDef *USARTx_DMA_RX_Channel);
static void NVIC_Configuration(void);

#endif /* __DMA_H */
