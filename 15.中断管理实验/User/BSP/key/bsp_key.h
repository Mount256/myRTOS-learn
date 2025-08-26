#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"

#define KEY0   GPIOE, GPIO_Pin_4
#define KEY1   GPIOE, GPIO_Pin_3

#define KEY_ON  0
#define KEY_OFF 1

void    Key_Init(void);
static void    EXTI_NVIC_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin_x);

#endif /* __KEY_H */
