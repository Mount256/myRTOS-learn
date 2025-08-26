/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
************************************************************************
*                           LOCAL  DEFINES
*************************************************************************
*/
extern uint8_t  ReceiveBuffer[RECEIVE_BUFFER_SIZE];
extern uint16_t USART1_RX_Sta; 

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskUsartTCB;
static OS_TCB   AppTaskKey0TCB;
static OS_TCB   AppTaskKey1TCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskUsartStk[APP_TASK_USART_STK_SIZE];
static CPU_STK   AppTaskKey0Stk [APP_TASK_KEY0_STK_SIZE];
static CPU_STK   AppTaskKey1Stk [APP_TASK_KEY1_STK_SIZE];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskUsart (void *p_arg);
static void AppTaskKey0  (void *p_arg);
static void AppTaskKey1  (void *p_arg);

/*
*************************************************************************
* 函数名 : main
* 描述   : 标准的C函数入口
* 形参   : 无
* 返回值 : 无
************************************************************************
*/
int main (void)
{
	OS_ERR err;
	
	BSP_Init ();
	OSInit (&err);
	
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskStartTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Start",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskStart,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_START_PRIO,			/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskStartStk[0],			/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_START_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_START_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	if (err != OS_ERR_NONE)
	{
		while(1);
	}
				  
	OSStart (&err);  /* Start multitasking (i.e. give control to uC/OS-III). */
	
	if (err != OS_ERR_NONE)
	{
		while(1);
	}
}

/*
**********************************************************************
* 函数名：AppTaskStart
* 描述   : 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在 BSP_Init 中实现)。
* 形参   : p_arg   是OSTaskCreate()在创建该任务时传递过来的形参。
* 返回值 : 无
* 注意   : 1) 第一行代码 (void)p_arg; 是为了防止编译器报错，因为形参p_arg并没有用到
**********************************************************************
*/
static void AppTaskStart (void *arg)
{
	CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
   (void) 		arg;
	
    BSP_Init(); // 开发板硬件初始化      
  
    CPU_Init(); // 初始化CPU组件（时间戳、关中断时间测量和主机名）

    cpu_clk_freq = BSP_CPU_ClkFreq();  // 获取CPU内核时钟频率                      
    cnts = cpu_clk_freq / (CPU_INT32U) OSCfg_TickRate_Hz;   // 根据用户设定的时钟节拍频率计算SysTick定时器的计数值     
    OS_CPU_SysTickInit (cnts); // 调用SysTick初始化函数，设置定时器计数值和启动定时器                            
     
    Mem_Init();   // 初始化内存管理组件（堆内存池和内存池表） 
	
#if OS_CFG_STAT_TASK_EN > 0u  
    OSStatTaskCPUUsageInit (&err);    // 计算没有应用任务（只有空闲任务）运行时CPU的最大容量                           
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();  // 复位（清零）当前最大关中断时间
#endif
	
	/********USART TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskUsartTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Usart",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskUsart,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_USART_PRIO,		    /* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskUsartStk[0],			/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_USART_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_USART_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
	
	/********KEY0 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey0TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Key0",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskKey0,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY0_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskKey0Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY0_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY0_STK_SIZE,		    /* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********KEY1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey1TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Key1",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskKey1,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY1_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskKey1Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY1_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY1_STK_SIZE,		    /* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	OSTaskDel (&AppTaskStartTCB, &err);
}

/*
************************************************************************
*                              USART TASK
***********************************************************************
*/
static  void  AppTaskUsart ( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
    (void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      
		OSTaskSemPend ((OS_TICK   )0,                     // 无期限等待
					   (OS_OPT    )OS_OPT_PEND_BLOCKING,  // 如果信号量不可用就等待
					   (CPU_TS   *)0,                     // 获取信号量被发布的时间戳
					   (OS_ERR   *)&err);                 // 返回错误类型

		OS_CRITICAL_ENTER();                              // 进入临界段，避免串口打印被打断
		printf("\r\nAppTaskUsart:接收数据内容:%s", ReceiveBuffer);
		printf("\r\nAppTaskUsart:接收数据长度:%d\r\n", USART1_RX_Sta);
		memset(ReceiveBuffer, 0, RECEIVE_BUFFER_SIZE);    // 清空接收缓冲区
		OS_CRITICAL_EXIT();                               // 退出临界段
    }
}

/*
***********************************************************************
*                         KEY0 TASK
**********************************************************************
*/
static  void  AppTaskKey0 ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	(void)         p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		OSTaskSemPend ((OS_TICK   )0,                     // 无期限等待
					   (OS_OPT    )OS_OPT_PEND_BLOCKING,  // 如果信号量不可用就等待
					   (CPU_TS   *)0,                     // 获取信号量被发布的时间戳
					   (OS_ERR   *)&err);                 // 返回错误类型
		
		LED0_REVERSE;
		ts_int = CPU_IntDisMeasMaxGet ();                 // 获取最大关中断时间

		OS_CRITICAL_ENTER();                              // 进入临界段，避免串口打印被打断
		printf ("\r\nAppTaskKey0:触发按键中断,最大中断时间是%dus\r\n",  ts_int / (cpu_clk_freq/1000000));        			
		OS_CRITICAL_EXIT();                               // 退出临界段
	}
}

/*
***********************************************************************
*                         KEY1 TASK
**********************************************************************
*/
static  void  AppTaskKey1 ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	(void)         p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		OSTaskSemPend ((OS_TICK   )0,                     // 无期限等待
					   (OS_OPT    )OS_OPT_PEND_BLOCKING,  // 如果信号量不可用就等待
					   (CPU_TS   *)0,                     // 获取信号量被发布的时间戳
					   (OS_ERR   *)&err);                 // 返回错误类型
		
		LED1_REVERSE;
		ts_int = CPU_IntDisMeasMaxGet ();                 // 获取最大关中断时间

		OS_CRITICAL_ENTER();                              // 进入临界段，避免串口打印被打断
		printf ("\r\nAppTaskKey1:触发按键中断,最大中断时间是%dus\r\n",  ts_int / (cpu_clk_freq/1000000));        			
		OS_CRITICAL_EXIT();                               // 退出临界段
	}
}

/*
************************************************************************
*                       INTERRUPT  HANDLER
***********************************************************************
*/
void EXTI3_IRQHandler(void)
{
	OS_ERR  err;
	
	OSIntEnter();  // 进入中断
	
	if (GPIO_ReadInputDataBit(KEY1) == KEY_ON)
	{
		while (GPIO_ReadInputDataBit(KEY1) == KEY_ON);  // 等待按键被松开
		/* 发送任务信号量到任务 AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskKey1TCB,   // 目标任务
					  (OS_OPT   )OS_OPT_POST_NONE,  // 没选项要求
					  (OS_ERR  *)&err);             // 返回错误类型		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line3);
	OSIntExit();   // 退出中断
}


void EXTI4_IRQHandler(void)
{
	OS_ERR  err;
	
	OSIntEnter();  // 进入中断
	
	if (GPIO_ReadInputDataBit(KEY0) == KEY_ON)
	{
		while (GPIO_ReadInputDataBit(KEY0) == KEY_ON);  // 等待按键被松开
		/* 发送任务信号量到任务 AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskKey0TCB,   // 目标任务
					  (OS_OPT   )OS_OPT_POST_NONE,  // 没选项要求
					  (OS_ERR  *)&err);             // 返回错误类型
	}
	
	EXTI_ClearITPendingBit(EXTI_Line4);
	OSIntExit();   // 退出中断
}


void USART1_IRQHandler(void)
{
	OS_ERR  err;
	
	OSIntEnter();  // 进入中断
 
    // 判断是否为空闲中断
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) 
	{
		// 计算本次接收到的数据长度
        USART1_RX_Sta = USART_Get_RX_Len(USART1_DMA_RX_Channel); // 将长度存入状态变量
		
        DMA_Cmd(USART1_DMA_RX_Channel, DISABLE);
        USART_ReceiveData(USART1);     // 清除 USART 的空闲中断标志位（通过读数据寄存器）
        DMA_ClearFlag(DMA1_FLAG_TC5);  // 清除 DMA 的标志位
        USART1_DMA_RX_Channel->CNDTR = RECEIVE_BUFFER_SIZE;  // 重新赋值计数值
        DMA_Cmd(USART1_DMA_RX_Channel, ENABLE);
 
		// 给出信号量，发送接收到新数据标志，供前台程序查询
        OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   // 目标任务
					  (OS_OPT   )OS_OPT_POST_NONE,   // 没选项要求
					  (OS_ERR  *)&err);              // 返回错误类型
    }
	
	OSIntExit();   // 退出中断
}
