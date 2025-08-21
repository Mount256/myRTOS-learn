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

CPU_TS             ts_start;       // 时间戳变量
CPU_TS             ts_end; 

CPU_TS             ts2_start;       // 时间戳变量
CPU_TS             ts2_end; 

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskTmrTCB;
static OS_TCB   AppTaskTmr2TCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskTmrStk  [ APP_TASK_TMR_STK_SIZE ];
static CPU_STK   AppTaskTmr2Stk [ APP_TASK_TMR2_STK_SIZE];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void  AppTaskTmr  (void *p_arg);
static void  AppTaskTmr2  (void *p_arg);

// 回调函数
void TmrCallback (OS_TMR *p_tmr, void *p_arg);
void Tmr2Callback (OS_TMR *p_tmr, void *p_arg);

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
	
	/********TMR TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskTmrTCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Timer",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskTmr,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_TMR_STK_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskTmrStk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_TMR_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_TMR_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********TMR2 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskTmr2TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Timer 2",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskTmr2,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_TMR2_STK_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskTmr2Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_TMR2_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_TMR2_STK_SIZE,		/* 任务栈大小 		*/
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
*                          TIMER TASK
***********************************************************************
*/
static  void  AppTaskTmr  ( void * p_arg )
{
	OS_ERR      err;
	OS_TMR      my_tmr;
    (void)      p_arg;

      
	/* 创建软件定时器 */
	OSTmrCreate ((OS_TMR              *)&my_tmr,             // 软件定时器对象
				 (CPU_CHAR            *)"MySoftTimer",       // 命名软件定时器
                 (OS_TICK              )10,                  // 定时器初始值(dly)，依10Hz时基(1 tick)计算，即1s(=10*100ms)
                 (OS_TICK              )10,                  // 定时器周期重载值(period)，依10Hz时基(1 tick)计算，即1s(=10*100ms)
                 (OS_OPT               )OS_OPT_TMR_PERIODIC, // 工作模式设置为周期性定时
                 (OS_TMR_CALLBACK_PTR  )TmrCallback,         // 回调函数指针
                 (void                *)"Timer Over!",       // 传递实参给回调函数
                 (OS_ERR              *)err);                // 返回错误类型
					 
	/* 启动软件定时器 */						 
	OSTmrStart ((OS_TMR   *)&my_tmr, // 软件定时器对象
                (OS_ERR   *)err);    // 返回错误类型
				 
	ts_start = OS_TS_GET();                       // 获取定时前时间戳
					 
    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{    
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, &err ); // 不断阻塞该任务
	}
}


static  void  AppTaskTmr2  ( void * p_arg )
{
	OS_ERR      err;
	OS_TMR      my_tmr2;
    (void)      p_arg;

      
	/* 创建软件定时器 */
	OSTmrCreate ((OS_TMR              *)&my_tmr2,             // 软件定时器对象
				 (CPU_CHAR            *)"MySoftTimer2",       // 命名软件定时器
                 (OS_TICK              )10,                  // 定时器初始值(dly)，依10Hz时基(1 tick)计算，即1s(=10*100ms)
                 (OS_TICK              )20,                  // 定时器周期重载值(period)，依10Hz时基(1 tick)计算，即2s(=20*100ms)
                 (OS_OPT               )OS_OPT_TMR_PERIODIC, // 工作模式设置为周期性定时
                 (OS_TMR_CALLBACK_PTR  )Tmr2Callback,         // 回调函数指针
                 (void                *)"Timer2 Over!",       // 传递实参给回调函数
                 (OS_ERR              *)err);                // 返回错误类型
					 
	/* 启动软件定时器 */						 
	OSTmrStart ((OS_TMR   *)&my_tmr2, // 软件定时器对象
                (OS_ERR   *)err);    // 返回错误类型
				 
	ts2_start = OS_TS_GET();                       // 获取定时前时间戳
					 
    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{    
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, &err ); // 不断阻塞该任务
	}
}

/*
************************************************************************
*                          TIMER CALLBACK
***********************************************************************
*/
void TmrCallback (OS_TMR *p_tmr, void *p_arg) // 软件定时器my_tmr的回调函数
{
	CPU_INT32U   cpu_clk_freq;	
	CPU_SR_ALLOC();      
	
	printf ("%s", (char *)p_arg);
	
	cpu_clk_freq = BSP_CPU_ClkFreq();                   // 获取CPU时钟，时间戳是以该时钟计数
	
	LED1_REVERSE; 
	
	ts_end = OS_TS_GET() - ts_start;     // 获取定时后的时间戳（以CPU时钟进行计数的一个计数值），并计算定时时间
	                                     
	OS_CRITICAL_ENTER();                 
	printf ("\r\n定时1s，通过时间戳测得定时 %07d us，即 %04d ms。\r\n", 
						ts_end / ( cpu_clk_freq / 1000000 ),     // 将定时时间折算成 us 
						ts_end / ( cpu_clk_freq / 1000 ) );      // 将定时时间折算成 ms 
	OS_CRITICAL_EXIT();                               

	ts_start = OS_TS_GET();                            // 获取定时前时间戳
}


void Tmr2Callback (OS_TMR *p_tmr, void *p_arg) // 软件定时器my_tmr的回调函数
{
	CPU_INT32U   cpu_clk_freq;	
	CPU_SR_ALLOC();      
	
	printf ("%s", (char *)p_arg);
	
	cpu_clk_freq = BSP_CPU_ClkFreq();                   // 获取CPU时钟，时间戳是以该时钟计数
	
	LED0_REVERSE; 
	
	ts2_end = OS_TS_GET() - ts2_start;     // 获取定时后的时间戳（以CPU时钟进行计数的一个计数值），并计算定时时间
	                                     
	OS_CRITICAL_ENTER();                 
	printf ("\r\n定时2s，通过时间戳测得定时 %07d us，即 %04d ms。\r\n", 
						ts2_end / ( cpu_clk_freq / 1000000 ),     // 将定时时间折算成 us 
						ts2_end / ( cpu_clk_freq / 1000 ) );      // 将定时时间折算成 ms 
	OS_CRITICAL_EXIT();                               

	ts2_start = OS_TS_GET();                            // 获取定时前时间戳
}
