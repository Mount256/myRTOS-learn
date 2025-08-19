/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
**************************************************************************
*                          LOCAL DEFINES
**************************************************************************
*/
OS_SEM SemOfKey;          // 标志KEY1是否被单击的多值信号量

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskKey0TCB;
static OS_TCB   AppTaskKey1TCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskKey0Stk [APP_TASK_KEY0_STK_SIZE ];
static CPU_STK   AppTaskKey1Stk [APP_TASK_KEY1_STK_SIZE ];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
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

	/* 创建多值信号量 SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,  /* 指向信号量变量的指针 */
                (CPU_CHAR    *)"SemOfKey", /* 信号量的名字         */
                (OS_SEM_CTR   )5,          /* 表示现有资源数目     */ 
                (OS_ERR      *)&err);      /* 错误类型             */
	
	/********KEY0 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey0TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task KEY0",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskKey0,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY0_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskKey0Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY0_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY0_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
	
	/********KEY1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey1TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task KEY1",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskKey1,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY1_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskKey1Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY1_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY1_STK_SIZE,		/* 任务栈大小 		*/
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
*                              KEY0 TASK
***********************************************************************
*/
static  void  AppTaskKey0 ( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();  // 需要调用临界段时必须定义 cpu_sr
	OS_SEM_CTR  ctr;
    (void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      
		if ( Key_Scan(KEY0) == KEY_ON )  // 如果KEY0被按下
		{ 
			ctr = OSSemPend ((OS_SEM   *)&SemOfKey,// 等待该信号量 SemOfKey
                             (OS_TICK   )0,        // 下面选择不等待，该参无效
                             (OS_OPT    )OS_OPT_PEND_NON_BLOCKING, // 如果没有信号量可用则不等待
                             (CPU_TS   *)0,        // 不获取时间戳
                             (OS_ERR   *)&err);    // 返回错误类型

			OS_CRITICAL_ENTER();   // 进入临界段，不希望打印串口被打断
			if ( err == OS_ERR_NONE )
				printf ( "\r\nKEY0被按下：成功申请到一个停车位，剩下%d个停车位。\r\n", ctr );
			else if ( err == OS_ERR_PEND_WOULD_BLOCK )
				printf ( "\r\nKEY0被按下：不好意思，现在停车场已满，请等待！\r\n" );
			OS_CRITICAL_EXIT();
		}

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, &err ); // 每20ms扫描一次
    }
}

/*
***********************************************************************
*                         KEY1 TASK
**********************************************************************
*/
static  void  AppTaskKey1 ( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();  // 需要调用临界段时必须定义 cpu_sr
	OS_SEM_CTR  ctr;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		if ( Key_Scan(KEY1) == KEY_ON ) // 如果KEY1被按下
		{ 
			ctr = OSSemPost((OS_SEM  *)&SemOfKey,       // 发布 SemOfKey
                            (OS_OPT   )OS_OPT_POST_ALL, // 发布给所有等待任务
                            (OS_ERR  *)&err);           // 返回错误类型

			OS_CRITICAL_ENTER();   // 进入临界段，不希望打印串口被打断
			if ( err == OS_ERR_NONE )
				printf ( "\r\nKEY1被按下：释放一个停车位，剩下%d个停车位。\r\n", ctr );
			OS_CRITICAL_EXIT();
		}

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, &err ); // 每20ms扫描一次
	}
}

