#include "bsp_dma.h"
#include "bsp_usart.h"

uint8_t  SendBuffer[SEND_BUFFER_SIZE];
uint8_t  ReceiveBuffer[RECEIVE_BUFFER_SIZE];
									
uint16_t USART1_RX_Sta = 0;      // 接收状态标志位
// 使用一个16位的变量来存储状态和长度
// bit15:   接收完成标志
// bit14:   接收到0x0D（\r）标志
// bit13~0: 接收到的有效数据长度


// 配置 NVIC
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 嵌套向量中断控制器组选择 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	// 配置 USART1 发送  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
    NVIC_Init(&NVIC_InitStructure);    
	
	// 配置 USART1 接收 
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
    NVIC_Init(&NVIC_InitStructure);      
}									
							

// M2P & P2M
void USART_DMA_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/***** (本实验用不上) 配置USART1发送 (USART1_TX的DMA请求位于DMA1 Channel 4) *****/
	DMA_DeInit(USART1_DMA_TX_Channel);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0; // 发送时再设置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // 存储器到外设模式
	
	DMA_InitStructure.DMA_BufferSize = 0;  // 发送时再设置
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(USART1_DMA_TX_Channel, &DMA_InitStructure);
	DMA_ITConfig(USART1_DMA_TX_Channel, DMA_IT_TC, ENABLE);
	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_Cmd(USART1_DMA_TX_Channel, ENABLE);
	
	/***** 配置USART1接收 (USART1_RX的DMA请求位于DMA1 Channel 5) *****/
	DMA_DeInit(USART1_DMA_RX_Channel);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ReceiveBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 外设到存储器模式
	
	DMA_InitStructure.DMA_BufferSize = RECEIVE_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(USART1_DMA_RX_Channel, &DMA_InitStructure);
	DMA_ITConfig(USART1_DMA_RX_Channel, DMA_IT_TC, ENABLE);
	DMA_ClearFlag(DMA1_FLAG_TC5);
	DMA_Cmd(USART1_DMA_RX_Channel, ENABLE);
	
	// 连接 USART 和 DMA，并使能
	//USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	
	NVIC_Configuration();
}


uint16_t USART_Get_RX_Len(DMA_Channel_TypeDef *USARTx_DMA_RX_Channel)
{
    // 接收缓冲区总长度 - DMA通道当前剩余的数据量 = 已接收的数据量
    return RECEIVE_BUFFER_SIZE - DMA_GetCurrDataCounter(USARTx_DMA_RX_Channel);
}

// 本实验用不上
void USART_DMA_Send(uint8_t *data, uint16_t len) 
{
	// 等待上一次DMA发送完成
    while (DMA_GetCurrDataCounter(USART1_DMA_TX_Channel) > 0);  // 简单等待，可优化为超时退出
 
    DMA_Cmd(USART1_DMA_TX_Channel, DISABLE);                    // 关闭DMA通道
    USART1_DMA_TX_Channel->CNDTR = len;                         // 设置传输数据量
    USART1_DMA_TX_Channel->CMAR = (uint32_t)data;               // 设置内存地址
    DMA_Cmd(USART1_DMA_TX_Channel, ENABLE);                     // 开启DMA通道，开始传输
}
