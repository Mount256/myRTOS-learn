#include "bsp_usart.h"


void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	// USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// USART1-TX发送 GPIOA.9 复用推挽模式
	GPIO_InitStructure.GPIO_Pin = USART1_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStructure);
	
	// USART1-RX接收 GPIOA.10 浮空输入模式
	GPIO_InitStructure.GPIO_Pin = USART1_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置USART1串口参数
	USART_InitStructure.USART_BaudRate = USART_BAUDRATE;			// 配置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 配置数据字长
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			// 配置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				// 配置校验位
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// 配置工作模式
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 配置硬件控制流
	USART_Init(USART1, &USART_InitStructure);						// 初始化串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);	 									// 使能串口
}


// 串口发送一个字节
void USART_SendByte(USART_TypeDef* pUSARTx, uint8_t data)
{
	USART_SendData(pUSARTx, data);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

// 串口发送两个字节
void USART_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t data)
{
	uint8_t temp_h, temp_l;
	temp_h = (data & 0xFF00) >> 8;
	temp_l = data & 0x00FF;
	
	USART_SendData(pUSARTx, temp_h);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	USART_SendData(pUSARTx, temp_l);
	while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

// 串口发送8位数据的数组
void USART_SendArray(USART_TypeDef* pUSARTx, uint8_t* Array, uint8_t num)
{
	uint8_t i;
	for( i = 0; i < num; i++)
	{
		USART_SendData(pUSARTx, Array[i]);
		while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	}
}

// 串口发送字符串
void USART_SendStr(USART_TypeDef* pUSARTx, uint8_t* str)
{
	while( *(str) != '\0')
	{
		USART_SendData(pUSARTx, *(str++));
		while(USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	}
}


//重定向c库函数printf到串口，重定向后可使用printf、putchar函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口 */
	USART_SendData(USART1, (uint8_t)ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}

//重定向c库函数scanf到串口，重写向后可使用scanf、getchar函数
int fgetc(FILE *f)
{
	/* 等待串口输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART1);
}
