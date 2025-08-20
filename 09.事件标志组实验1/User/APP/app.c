/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
OS_FLAG_GRP flag_grp;            // 声明事件标志组

#define KEY0_EVENT  (0x01 << 0)  // 设置事件掩码的位0
#define KEY1_EVENT  (0x01 << 1)  // 设置事件掩码的位1

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskPostTCB;
static OS_TCB   AppTaskPendTCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskPostStk [APP_TASK_POST_STK_SIZE ];
static CPU_STK   AppTaskPendStk [APP_TASK_PEND_STK_SIZE ];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost  (void *p_arg);
static void AppTaskPend  (void *p_arg);

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

	 /* 创建事件标志组 flag_grp */
	OSFlagCreate ((OS_FLAG_GRP  *)&flag_grp,        // 指向事件标志组的指针
                  (CPU_CHAR     *)"FLAG For Test",  // 事件标志组的名字
                  (OS_FLAGS      )0,                // 事件标志组的初始值
                  (OS_ERR       *)&err);			// 返回错误类型
	
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
	
	/********PEND TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPendTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Pend",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskPend,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_PEND_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskPendStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_PEND_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_PEND_STK_SIZE,		/* 任务栈大小 		*/
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
    (void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      
		if( Key_Scan(KEY0) == KEY_ON ) // 如果KEY0被按下
		{		                                                   
			printf("\r\nKEY0被按下\r\n");
			OSFlagPost ((OS_FLAG_GRP  *)&flag_grp,                             
                        (OS_FLAGS      )KEY0_EVENT,
                        (OS_OPT        )OS_OPT_POST_FLAG_SET,  // 将标志组的BIT0置1
                        (OS_ERR       *)&err);

		}

		if( Key_Scan(KEY1) == KEY_ON ) // 如果KEY1被按下
		{		                                                   
			printf("\r\nKEY1被按下\r\n");
			OSFlagPost ((OS_FLAG_GRP  *)&flag_grp,                             
                        (OS_FLAGS      )KEY1_EVENT,
                        (OS_OPT        )OS_OPT_POST_FLAG_SET,  // 将标志组的BIT1置1
                        (OS_ERR       *)&err);

		}

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );  // 每20ms扫描一次
    }
}

/*
***********************************************************************
*                         PEND TASK
**********************************************************************
*/
static  void  AppTaskPend ( void * p_arg )
{
	OS_ERR      err;
	OS_FLAGS      flags_rdy;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		// 等待标志组的的BIT0和BIT1均被置1 
		flags_rdy = OSFlagPend ((OS_FLAG_GRP *)&flag_grp,                 
                                (OS_FLAGS     )( KEY0_EVENT | KEY1_EVENT ),
                                (OS_TICK      )0,
                                (OS_OPT       )OS_OPT_PEND_FLAG_SET_ALL |  // 要求所有等待标志位均要置1
		                                       OS_OPT_PEND_BLOCKING |
                                               OS_OPT_PEND_FLAG_CONSUME,   // 事件等待成功后，必须使用该选项显式清除已接收到的事件类型
                                (CPU_TS      *)0,
                                (OS_ERR      *)&err);
		
		if ( (flags_rdy & (KEY0_EVENT|KEY1_EVENT)) == (KEY0_EVENT|KEY1_EVENT) ) 
		{
			/* 如果接收完成并且正确 */
			printf ("\r\nKEY0与KEY1都按下\r\n");		
			LED0_REVERSE;       
		}
		
	}
}
