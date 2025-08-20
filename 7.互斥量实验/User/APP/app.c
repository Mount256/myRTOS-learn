/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
*******************************************************************
*                                            LOCAL DEFINES
*******************************************************************
*/

OS_MUTEX TestMutex;      // 互斥量   

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskHighTCB;
static OS_TCB   AppTaskMedTCB;
static OS_TCB	AppTaskLowTCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskHighStk [APP_TASK_HIGH_STK_SIZE ];
static CPU_STK   AppTaskMedStk [APP_TASK_MED_STK_SIZE ];
static CPU_STK	 AppTaskLowStk [APP_TASK_LOW_STK_SIZE ];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskHigh  (void *p_arg);
static void AppTaskMed  (void *p_arg);
static void AppTaskLow  (void *p_arg);

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

	/* 创建互斥信号量 Mutex */
	OSMutexCreate ((OS_MUTEX  *)&TestMutex,       // 指向信号量变量的指针
                   (CPU_CHAR  *)"Mutex For Test", // 信号量的名字
                   (OS_ERR    *)&err);            // 错误类型
	
	/********HIGH TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskHighTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task High",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskHigh,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_HIGH_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskHighStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_HIGH_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_HIGH_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
	
	/********MED TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskMedTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Med",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskMed,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_MED_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskMedStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_MED_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_MED_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********LOW TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskLowTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Low",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskLow,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_LOW_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskLowStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_LOW_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_LOW_STK_SIZE,		/* 任务栈大小 		*/
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
*                              HIGH TASK
***********************************************************************
*/
static  void  AppTaskHigh ( void * p_arg )
{
	OS_ERR      err;
	//static uint32_t  i;
	//CPU_TS      ts_sem_post;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		// 获取互斥量，若未获取到则一直等待
		printf("\r\nAppTaskHigh 准备获取互斥量！\r\n");
		OSMutexPend ((OS_MUTEX *)&TestMutex,  //等待该互斥量被发布
                     (OS_TICK   )0,           //无期限等待
                     (OS_OPT    )OS_OPT_PEND_BLOCKING,  // 如果没有信号量可用就等等
                     (CPU_TS   *)0,           // 不需要时间戳
                     (OS_ERR   *)&err);       // 返回错误类型
		
		printf("\r\nAppTaskHigh 已获取互斥量，正在运行中。\r\n");
		//for (i=0; i<3000000; i++);
		//OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err); 
		delay_ms(1000);	

		// 发布互斥量
		printf("\r\nAppTaskHigh 释放了互斥量!\r\n");
        OSMutexPost((OS_MUTEX*)&TestMutex,
                    (OS_OPT   )OS_OPT_POST_NONE, // 进行任务调度
                    (OS_ERR  *)&err);
		
        OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err);  // 延时 1s
    }
}

/*
***********************************************************************
*                         MED TASK
**********************************************************************
*/
static  void  AppTaskMed ( void * p_arg )
{
	OS_ERR      err;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		printf("\r\nAppTaskMed 正在运行中。\r\n");
        OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err);
	}
}

/*
***********************************************************************
*                         LOW TASK
**********************************************************************
*/
static  void  AppTaskLow ( void * p_arg )
{
	OS_ERR           err;
	//static uint32_t  i;
	//CPU_TS           ts_sem_post;
    (void)           p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		// 获取互斥量，若未获取到则一直等待
		printf("\r\nAppTaskLow 准备获取互斥量！\r\n");
		OSMutexPend ((OS_MUTEX *)&TestMutex,  //等待该互斥量被发布
                     (OS_TICK   )0,           //无期限等待
                     (OS_OPT    )OS_OPT_PEND_BLOCKING,  // 如果没有信号量可用就等等
                     (CPU_TS   *)0,           // 不需要时间戳
                     (OS_ERR   *)&err);       // 返回错误类型
		
		printf("\r\nAppTaskLow 已获取互斥量，正在运行中。\r\n");
		//for (i=0; i<3000000; i++);
		//OSTimeDlyHMSM (0, 0, 3, 0, OS_OPT_TIME_PERIODIC, &err);  
		delay_ms(3000);
		
		// 发布互斥量
		printf("\r\nAppTaskLow 释放了互斥量!\r\n");
        OSMutexPost((OS_MUTEX*)&TestMutex,
                    (OS_OPT   )OS_OPT_POST_NONE, // 进行任务调度
                    (OS_ERR  *)&err);

        OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err);  // 延时 1s
	}
}
