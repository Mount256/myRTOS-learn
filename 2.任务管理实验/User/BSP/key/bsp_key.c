#include "bsp_key.h"

void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE, ENABLE);
	
	/* KEY0 & KEY1 -- Ground */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4; /* PinE3 -- KEY1, PinE4 -- KEY0 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; /* 上拉输入 */
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	/* KEY_UP(WK_UP) -- VCC */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; /* PinA0 -- KEY_UP */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; /* 下拉输入 */
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
