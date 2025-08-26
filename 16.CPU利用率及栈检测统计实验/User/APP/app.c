/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
*************************************************************************
*                        LOCAL DEFINES
*************************************************************************
*/
OS_Q queue;     //声明消息队列

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskPostTCB;
static OS_TCB   AppTaskPend1TCB;
static OS_TCB   AppTaskPend2TCB;
static OS_TCB   AppTaskStatusTCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk [APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskPostStk  [APP_TASK_POST_STK_SIZE ];
static CPU_STK   AppTaskPend1Stk [APP_TASK_PEND1_STK_SIZE];
static CPU_STK   AppTaskPend2Stk [APP_TASK_PEND2_STK_SIZE];
static CPU_STK   AppTaskStatusStk[APP_TASK_STATUS_STK_SIZE];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost  (void *p_arg);
static void AppTaskPend1 (void *p_arg);
static void AppTaskPend2 (void *p_arg);
static void AppTaskStatus(void *p_arg);

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

	/********QUEUE CREATE*********/
	OSQCreate ((OS_Q       *)&queue,            /* 指向消息队列的指针 */
	           (CPU_CHAR   *)"Queue for test",  /* 队列的名字 		*/
			   (OS_MSG_QTY  )20,                /* 最多可存放消息的数目 */
			   (OS_ERR     *)&err);			    /* 返回错误类型 		*/
	
	/********POST TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPostTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Post",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskPost,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_POST_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskPostStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_POST_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_POST_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
	
	/********PEND1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPend1TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Pend 1",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskPend1,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_PEND1_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskPend1Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_PEND1_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_PEND1_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********PEND2 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPend2TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Pend 2",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskPend2,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_PEND2_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskPend2Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_PEND2_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_PEND2_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********STATUS TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskStatusTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Status",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskStatus,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_STATUS_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskStatusStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_STATUS_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_STATUS_STK_SIZE,		/* 任务栈大小 		*/
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
*                              POST TASK
***********************************************************************
*/
static  void  AppTaskPost ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // 需要调用临界段时必须定义 cpu_sr
	char        msg[] = "你好，我是uC/OS！";  // 消息数据
	
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      		
	   /* 发布消息到消息队列 queue */
		OSQPost ((OS_Q        *)&queue,          /* 消息变量指针 */
                 (uint8_t     *)msg,             /* 要发送的数据的指针，将内存地址通过队列“发送出去”*/
                 (OS_MSG_SIZE  )sizeof (msg),    /* 数据字节大小 */
                 //(OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, /* 先进先出和发布给全部任务的形式 */
				 (OS_OPT       )OS_OPT_POST_FIFO,   /* 仅先进先出的形式 */
                 (OS_ERR      *)&err);              /* 返回错误类型 */
				 
		if ( err == OS_ERR_NONE )  // 如果发送成功
		{    
			OS_CRITICAL_ENTER();   // 进入临界段，不希望打印串口被打断
			printf ( "\r\nAppTaskPost：发送消息的内容：%s\r\n", msg );
			OS_CRITICAL_EXIT();
        }
				
        OSTimeDly ( 1000, OS_OPT_TIME_DLY, &err ); 
    }
}

/*
***********************************************************************
*                         PEND1 TASK
**********************************************************************
*/
static  void  AppTaskPend1 ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // 需要调用临界段时必须定义 cpu_sr
	OS_MSG_SIZE msg_size;
	char*       msg;
	
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		/* 请求消息队列 queue 的消息 */
        msg = OSQPend ((OS_Q         *)&queue,     /* 消息变量指针 */
                       (OS_TICK       )0,          /* 等待时长为无限 */
                       (OS_OPT        )OS_OPT_PEND_BLOCKING, /* 如果没有获取到信号量就等待 */
                       (OS_MSG_SIZE  *)&msg_size,  /* 获取消息的字节大小 */
                       (CPU_TS       *)0,          /* 获取任务发送时的时间戳 */
                       (OS_ERR       *)&err);      /* 返回错误 */

		if ( err == OS_ERR_NONE )  // 如果接收成功
		{    
			OS_CRITICAL_ENTER();   // 进入临界段，不希望打印串口被打断
			printf ( "\r\nAppTaskPend1：接收消息的长度：%d字节，内容：%s\r\n", msg_size, msg );
			OS_CRITICAL_EXIT();
        }
	}
}

/*
***********************************************************************
*                         PEND2 TASK
**********************************************************************
*/
static  void  AppTaskPend2 ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // 需要调用临界段时必须定义 cpu_sr
	OS_MSG_SIZE msg_size;
	char*       msg;
	
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		/* 请求消息队列 queue 的消息 */
        msg = OSQPend ((OS_Q         *)&queue,     /* 消息变量指针 */
                       (OS_TICK       )0,          /* 等待时长为无限 */
                       (OS_OPT        )OS_OPT_PEND_BLOCKING, /* 如果没有获取到信号量就等待 */
                       (OS_MSG_SIZE  *)&msg_size,  /* 获取消息的字节大小 */
                       (CPU_TS       *)0,          /* 获取任务发送时的时间戳 */
                       (OS_ERR       *)&err);      /* 返回错误 */

		if ( err == OS_ERR_NONE )  // 如果接收成功
		{    
			OS_CRITICAL_ENTER();   // 进入临界段，不希望打印串口被打断
			printf ( "\r\nAppTaskPend2：接收消息的长度：%d字节，内容：%s\r\n", msg_size, msg );
			OS_CRITICAL_EXIT();
        }
	}
}

/*
***********************************************************************
*                         STATUS TASK
**********************************************************************
*/
static  void  AppTaskStatus ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // 需要调用临界段时必须定义 cpu_sr
	
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{  
		OS_CRITICAL_ENTER();
		printf("---------------------------------------------------\r\n");
        printf ( "CPU利用率：%d.%d%%\r\n",
            OSStatTaskCPUUsage / 100, OSStatTaskCPUUsage % 100 );
        printf ( "CPU最大利用率：%d.%d%%\r\n",
            OSStatTaskCPUUsageMax / 100, OSStatTaskCPUUsageMax % 100 );


        printf ( "Post任务的CPU利用率：%d.%d%%\r\n",
            AppTaskPostTCB.CPUUsageMax / 100, AppTaskPostTCB.CPUUsageMax % 100 );
        printf ( "Pend1任务的CPU利用率：%d.%d%%\r\n",
            AppTaskPend1TCB.CPUUsageMax / 100, AppTaskPend1TCB.CPUUsageMax % 100 );
        printf ( "Pend2任务的CPU利用率：%d.%d%%\r\n",
            AppTaskPend2TCB.CPUUsageMax / 100, AppTaskPend2TCB.CPUUsageMax % 100 );
        printf ( "Status任务的CPU利用率：%d.%d%%\r\n",
            AppTaskStatusTCB.CPUUsageMax / 100, AppTaskStatusTCB.CPUUsageMax % 100 ) ;


        printf ( "Post任务的已用和空闲栈大小分别为：%d, %d\r\n",
                AppTaskPostTCB.StkUsed, AppTaskPostTCB.StkFree );
        printf ( "Pend1任务的已用和空闲栈大小分别为：%d, %d\r\n",
                AppTaskPend1TCB.StkUsed, AppTaskPend1TCB.StkFree );
        printf ( "Pend2任务的已用和空闲栈大小分别为：%d, %d\r\n",
                AppTaskPend2TCB.StkUsed, AppTaskPend2TCB.StkFree );
        printf ( "Status任务的已用和空闲栈大小分别为：%d, %d\r\n",
                AppTaskStatusTCB.StkUsed, AppTaskStatusTCB.StkFree );

        printf("---------------------------------------------------\r\n");
        OS_CRITICAL_EXIT();                               

        OSTimeDlyHMSM (0, 0, 2, 0, OS_OPT_TIME_PERIODIC, &err);
	}
}
