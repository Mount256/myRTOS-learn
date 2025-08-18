/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskLed0TCB;
static OS_TCB   AppTaskLed1TCB;
static OS_TCB	AppTaskBeepTCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskLed0Stk [APP_TASK_LED0_STK_SIZE ];
static CPU_STK   AppTaskLed1Stk [APP_TASK_LED1_STK_SIZE ];
static CPU_STK	 AppTaskBeepStk [APP_TASK_BEEP_STK_SIZE ];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskLED0  (void *p_arg);
static void AppTaskLED1  (void *p_arg);
static void AppTaskBeep  (void *p_arg);

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
	
	/********LED0 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskLed0TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task LED0",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskLED0,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_LED0_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskLed0Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_LED0_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_LED0_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
	
	/********LED1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskLed1TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task LED1",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskLED1,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_LED1_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskLed1Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_LED1_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_LED1_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********BEEP TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskBeepTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Beep",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskBeep,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_BEEP_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskBeepStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_BEEP_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_BEEP_STK_SIZE,		/* 任务栈大小 		*/
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
*                              LED0 TASK
***********************************************************************
*/
static  void  AppTaskLED0 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
    (void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      
		LED0_REVERSE;    // 切换 LED0 的亮灭状态
        value = OSTaskRegGet ( 0, 0, & err );        // 获取自身任务寄存器值

		if ( value < 10 ) // 如果任务寄存器值<10
		{                          
			OSTaskRegSet ( 0, 0, ++ value, & err );    // 继续累加任务寄存器
        } 
		else  // 如果累加到10
		{                                     
            OSTaskRegSet ( 0, 0, 0, & err );           // 将任务寄存器值归0
            printf("\r\nAppTaskLED0：我恢复了LED1任务！\r\n");
            OSTaskResume ( & AppTaskLed1TCB, & err );  // 恢复 LED1 任务

            printf("\r\nAppTaskLED0：我恢复了Beep任务！\r\n");
            OSTaskResume ( & AppTaskBeepTCB, & err );  // 恢复 Beep 任务
        }
            OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );  // 相对性延时1000个节拍（1s）
    }
}

/*
***********************************************************************
*                         LED1 TASK
**********************************************************************
*/
static  void  AppTaskLED1 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		LED1_REVERSE;    // 切换 LED1 的亮灭状态
		value = OSTaskRegGet ( 0, 0, & err );      // 获取自身任务寄存器值

		if ( value < 5 ) 
		{                         // 如果任务寄存器值<5
			OSTaskRegSet ( 0, 0, ++ value, & err );  // 继续累加任务寄存器值
		} 
		else 
		{                                   // 如果累加到5
			OSTaskRegSet ( 0, 0, 0, & err );         // 将任务寄存器值归0
			printf("\r\nAppTaskLED1：我自己挂起LED1任务！\r\n");
			OSTaskSuspend ( 0, & err );              // 挂起自身
		}
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); // 相对性延时1000个时钟节拍（1s）
	}
}

/*
***********************************************************************
*                         BEEP TASK
**********************************************************************
*/
static  void  AppTaskBeep ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		BEEP_REVERSE;    // 切换 Beep 的响灭状态
		value = OSTaskRegGet ( 0, 0, & err );      // 获取自身任务寄存器值

		if ( value < 5 ) 
		{                         // 如果任务寄存器值<5
			OSTaskRegSet ( 0, 0, ++ value, & err );  // 继续累加任务寄存器值
		} 
		else 
		{                                   // 如果累加到5
			OSTaskRegSet ( 0, 0, 0, & err );         // 将任务寄存器值归0
			printf("\r\nAppTaskBeep：我自己挂起Beep任务！\r\n");
			OSTaskSuspend ( 0, & err );              // 挂起自身
		}
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); // 相对性延时1000个时钟节拍（1s）
	}
}
