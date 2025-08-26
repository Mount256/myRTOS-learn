#include "bsp_dma.h"
#include "bsp_usart.h"

uint8_t  SendBuffer[SEND_BUFFER_SIZE];
uint8_t  ReceiveBuffer[RECEIVE_BUFFER_SIZE];
									
uint16_t USART1_RX_Sta = 0;      // ����״̬��־λ
// ʹ��һ��16λ�ı������洢״̬�ͳ���
// bit15:   ������ɱ�־
// bit14:   ���յ�0x0D��\r����־
// bit13~0: ���յ�����Ч���ݳ���


// ���� NVIC
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Ƕ�������жϿ�������ѡ�� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	// ���� USART1 ����  
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;    
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
    NVIC_Init(&NVIC_InitStructure);    
	
	// ���� USART1 ���� 
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

	/***** (��ʵ���ò���) ����USART1���� (USART1_TX��DMA����λ��DMA1 Channel 4) *****/
	DMA_DeInit(USART1_DMA_TX_Channel);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0; // ����ʱ������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  // �洢��������ģʽ
	
	DMA_InitStructure.DMA_BufferSize = 0;  // ����ʱ������
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
	
	/***** ����USART1���� (USART1_RX��DMA����λ��DMA1 Channel 5) *****/
	DMA_DeInit(USART1_DMA_RX_Channel);
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_ADDRESS;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ReceiveBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // ���赽�洢��ģʽ
	
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
	
	// ���� USART �� DMA����ʹ��
	//USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	
	NVIC_Configuration();
}


uint16_t USART_Get_RX_Len(DMA_Channel_TypeDef *USARTx_DMA_RX_Channel)
{
    // ���ջ������ܳ��� - DMAͨ����ǰʣ��������� = �ѽ��յ�������
    return RECEIVE_BUFFER_SIZE - DMA_GetCurrDataCounter(USARTx_DMA_RX_Channel);
}

// ��ʵ���ò���
void USART_DMA_Send(uint8_t *data, uint16_t len) 
{
	// �ȴ���һ��DMA�������
    while (DMA_GetCurrDataCounter(USART1_DMA_TX_Channel) > 0);  // �򵥵ȴ������Ż�Ϊ��ʱ�˳�
 
    DMA_Cmd(USART1_DMA_TX_Channel, DISABLE);                    // �ر�DMAͨ��
    USART1_DMA_TX_Channel->CNDTR = len;                         // ���ô���������
    USART1_DMA_TX_Channel->CMAR = (uint32_t)data;               // �����ڴ��ַ
    DMA_Cmd(USART1_DMA_TX_Channel, ENABLE);                     // ����DMAͨ������ʼ����
}
