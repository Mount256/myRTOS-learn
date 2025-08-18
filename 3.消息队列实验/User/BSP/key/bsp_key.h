#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY0 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)
#define KEY_UP GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

#define IPU_ON 0
#define IPD_ON 1

#define ALL_KEY_UNPRS 0
#define KEY0_PRS 1
#define KEY1_PRS 2
#define KEY_UP_PRS 3

void Key_Init(void);
//uint_8 Key_Scan(void);

#endif /* __KEY_H */
