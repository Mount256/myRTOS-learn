/*
************************************************************************
*                              包含的文件
*************************************************************************
*/
#include <includes.h>

/*
*************************************************************************
*                            LOCAL DEFINES
*************************************************************************
*/
OS_MEM  mem;                    // 声明内存管理对象
uint8_t ucArray [5] [20];       // 声明内存分区大小

/*
*************************************************************************
*                                任务控制块TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskPost1TCB;
static OS_TCB   AppTaskPost2TCB;
static OS_TCB   AppTaskPendTCB;

/*
*************************************************************************
*                                 任务堆栈
*************************************************************************
*/
static CPU_STK   AppTaskStartStk [APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskPost1Stk  [APP_TASK_POST1_STK_SIZE ];
static CPU_STK   AppTaskPost2Stk  [APP_TASK_POST2_STK_SIZE ];
static CPU_STK   AppTaskPendStk [APP_TASK_PEND_STK_SIZE];

/*
************************************************************************
*                           函数原型
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost1  (void *p_arg);
static void AppTaskPost2  (void *p_arg);
static void AppTaskPend (void *p_arg);

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

	/* 创建内存管理对象 mem */
	OSMemCreate ((OS_MEM      *)&mem,             // 指向内存管理对象
				 (CPU_CHAR    *)"Mem For Test",   // 命名内存管理对象
				 (void        *)ucArray,          // 内存分区的首地址
				 (OS_MEM_QTY   )5,                // 内存分区中内存块数目为5
				 (OS_MEM_SIZE  )20,               // 内存块的字节数目为20
				 (OS_ERR      *)&err);            // 返回错误类型
	
	/********POST1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPost1TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Post 1",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskPost1,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_POST1_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskPost1Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_POST1_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_POST1_STK_SIZE,		/* 任务栈大小 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* 设置可以发送到任务的最大消息数 	*/
                  (OS_TICK       ) 	0u,				/* 在任务之间循环时的时间片的时间量 	*/					
                  (void       	*) 	0,				/* 指向用户提供的内存位置的指针，用作 TCB 扩展 */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* 任务特定选项 */
                  (OS_ERR     	*)	&err
				);
				  
	/********POST2 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPost2TCB,  				/* 任务控制块 		*/
				  (CPU_CHAR 	*)	"App Task Post 2",				/* 任务名称 			*/
				  (OS_TASK_PTR	 )	AppTaskPost2,					/* 任务函数名称 		*/
				  (void 		*)	0,								/* 任务入口函数形参 	*/
				  (OS_PRIO	 	 )	APP_TASK_POST2_PRIO,				/* 任务的优先级 		*/
				  (CPU_STK		*)	&AppTaskPost2Stk[0],				/* 栈的起始地址 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_POST2_STK_SIZE / 10, 	/* 任务栈的限制位置 	*/
				  (CPU_STK_SIZE  )	APP_TASK_POST2_STK_SIZE,		/* 任务栈大小 		*/
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
*                              POST1 TASK
***********************************************************************
*/
static  void  AppTaskPost1 ( void * p_arg )
{
	OS_ERR      err;
	char *      p_mem_blk;
	CPU_SR_ALLOC();
	uint32_t    ulCount = 0;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      		
	    p_mem_blk = OSMemGet ((OS_MEM      *)&mem,             // 指向内存管理对象
		                      (OS_ERR      *)&err);            // 返回错误类型
		
		OS_CRITICAL_ENTER();                              // 进入临界段，避免串口打印被打断
		printf ("\r\nAppTaskPost1已申请内存块！\r\n");
		OS_CRITICAL_EXIT();                               // 退出临界段
		
		sprintf (p_mem_blk, "%d", ulCount++);                  // 向内存块存取计数值
		
		/* 发布任务消息到任务 AppTaskPend */
		OSTaskQPost ((OS_TCB      *)&AppTaskPendTCB,           // 目标任务的控制块
					 (void        *)p_mem_blk,                 // 消息内容的首地址
					 (OS_MSG_SIZE  )strlen ( p_mem_blk ),      // 消息长度
					 (OS_OPT       )OS_OPT_POST_FIFO,          // 发布到任务消息队列的入口端
					 (OS_ERR      *)&err);                     // 返回错误类型
				
        OSTimeDly ( 1000, OS_OPT_TIME_DLY, &err ); 
    }
}

/*
***********************************************************************
*                         POST2 TASK
**********************************************************************
*/
static  void  AppTaskPost2 ( void * p_arg )
{
	OS_ERR      err;
	char *      p_mem_blk;
	CPU_SR_ALLOC();
	uint32_t    ulCount = 0;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{      		
	    p_mem_blk = OSMemGet ((OS_MEM      *)&mem,             // 指向内存管理对象
		                      (OS_ERR      *)&err);            // 返回错误类型
		
		OS_CRITICAL_ENTER();                              // 进入临界段，避免串口打印被打断
		printf ("\r\nAppTaskPost2已申请内存块！\r\n");
		OS_CRITICAL_EXIT();                               // 退出临界段
		
		sprintf (p_mem_blk, "%d", ulCount++);                  // 向内存块存取计数值
		
		/* 发布任务消息到任务 AppTaskPend */
		OSTaskQPost ((OS_TCB      *)&AppTaskPendTCB,           // 目标任务的控制块
					 (void        *)p_mem_blk,                 // 消息内容的首地址
					 (OS_MSG_SIZE  )strlen ( p_mem_blk ),      // 消息长度
					 (OS_OPT       )OS_OPT_POST_FIFO,          // 发布到任务消息队列的入口端
					 (OS_ERR      *)&err);                     // 返回错误类型
				
        OSTimeDly ( 1000, OS_OPT_TIME_DLY, &err ); 
    }
}

/*
***********************************************************************
*                         PEND TASK
**********************************************************************
*/
static  void  AppTaskPend ( void * p_arg )
{
	OS_ERR         err;
	OS_MSG_SIZE    msg_size;
	CPU_SR_ALLOC();
	char *      pMsg;
	(void)      p_arg;

    while (DEF_TRUE) // 任务体，通常写成一个死循环
	{   
		/* 阻塞任务，等待任务消息 */
		pMsg = OSTaskQPend ((OS_TICK        )0,                    // 无期限等待
							(OS_OPT         )OS_OPT_PEND_BLOCKING, // 没有消息就阻塞任务
							(OS_MSG_SIZE   *)&msg_size,            // 返回消息长度
							(CPU_TS        *)0,                    // 返回消息被发布的时间戳（不需要）
							(OS_ERR        *)&err);                // 返回错误类型
		
		LED1_REVERSE;                               
		
		OS_CRITICAL_ENTER();                              // 进入临界段，避免串口打印被打断
		printf ( "\r\n接收到的消息的内容为：%s，长度是：%d字节。", pMsg, msg_size );   
		printf ( "\r\nAppTaskPend释放内存块！\r\n" ); 		
		OS_CRITICAL_EXIT();                               // 退出临界段
		
		/* 退还内存块 */
		OSMemPut ((OS_MEM  *)&mem,                        // 指向内存管理对象
				  (void    *)pMsg,                        // 内存块的首地址
				  (OS_ERR  *)&err);		                  // 返回错误类型
	}
}
