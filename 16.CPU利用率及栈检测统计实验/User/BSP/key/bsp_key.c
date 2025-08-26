#include "bsp_key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	/* KEY0 & KEY1 -- Ground */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; /* PinE3 -- KEY1, PinE4 -- KEY0 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; /* 上拉输入 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);
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
