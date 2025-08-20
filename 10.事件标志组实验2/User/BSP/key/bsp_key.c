#include "bsp_key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	/* KEY0 & KEY1 -- Ground */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; /* PinE3 -- KEY1, PinE4 -- KEY0 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; /* 上拉输入 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void EXTI_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	/* init NVIC */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	
	/* init EXTI */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line4;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x)
{
	if (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin_x) == KEY_ON)
	{
		while (GPIO_ReadInputDataBit(GPIOx, GPIO_Pin_x) == KEY_ON);  // 等待按键被松开
		return KEY_ON;
	}
	else
		return KEY_OFF;
}
