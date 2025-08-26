/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
*************************************************************************
*                        LOCAL DEFINES
*************************************************************************
*/
OS_Q queue;     //������Ϣ����

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskPostTCB;
static OS_TCB   AppTaskPend1TCB;
static OS_TCB   AppTaskPend2TCB;
static OS_TCB   AppTaskStatusTCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk [APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskPostStk  [APP_TASK_POST_STK_SIZE ];
static CPU_STK   AppTaskPend1Stk [APP_TASK_PEND1_STK_SIZE];
static CPU_STK   AppTaskPend2Stk [APP_TASK_PEND2_STK_SIZE];
static CPU_STK   AppTaskStatusStk[APP_TASK_STATUS_STK_SIZE];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost  (void *p_arg);
static void AppTaskPend1 (void *p_arg);
static void AppTaskPend2 (void *p_arg);
static void AppTaskStatus(void *p_arg);

/*
*************************************************************************
* ������ : main
* ����   : ��׼��C�������
* �β�   : ��
* ����ֵ : ��
************************************************************************
*/
int main (void)
{
	OS_ERR err;
	
	BSP_Init ();
	OSInit (&err);
	
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskStartTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Start",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskStart,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_START_PRIO,			/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskStartStk[0],			/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_START_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_START_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
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
* ��������AppTaskStart
* ����   : ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(�� BSP_Init ��ʵ��)��
* �β�   : p_arg   ��OSTaskCreate()�ڴ���������ʱ���ݹ������βΡ�
* ����ֵ : ��
* ע��   : 1) ��һ�д��� (void)p_arg; ��Ϊ�˷�ֹ������������Ϊ�β�p_arg��û���õ�
**********************************************************************
*/
static void AppTaskStart (void *arg)
{
	CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
   (void) 		arg;
	
    BSP_Init(); // ������Ӳ����ʼ��      
  
    CPU_Init(); // ��ʼ��CPU�����ʱ��������ж�ʱ���������������

    cpu_clk_freq = BSP_CPU_ClkFreq();  // ��ȡCPU�ں�ʱ��Ƶ��                      
    cnts = cpu_clk_freq / (CPU_INT32U) OSCfg_TickRate_Hz;   // �����û��趨��ʱ�ӽ���Ƶ�ʼ���SysTick��ʱ���ļ���ֵ     
    OS_CPU_SysTickInit (cnts); // ����SysTick��ʼ�����������ö�ʱ������ֵ��������ʱ��                            
     
    Mem_Init();   // ��ʼ���ڴ������������ڴ�غ��ڴ�ر� 
	
#if OS_CFG_STAT_TASK_EN > 0u  
    OSStatTaskCPUUsageInit (&err);    // ����û��Ӧ������ֻ�п�����������ʱCPU���������                           
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
    CPU_IntDisMeasMaxCurReset();  // ��λ�����㣩��ǰ�����ж�ʱ��
#endif

	/********QUEUE CREATE*********/
	OSQCreate ((OS_Q       *)&queue,            /* ָ����Ϣ���е�ָ�� */
	           (CPU_CHAR   *)"Queue for test",  /* ���е����� 		*/
			   (OS_MSG_QTY  )20,                /* ���ɴ����Ϣ����Ŀ */
			   (OS_ERR     *)&err);			    /* ���ش������� 		*/
	
	/********POST TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPostTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Post",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskPost,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_POST_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskPostStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_POST_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_POST_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
	
	/********PEND1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPend1TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Pend 1",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskPend1,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_PEND1_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskPend1Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_PEND1_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_PEND1_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
				  
	/********PEND2 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPend2TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Pend 2",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskPend2,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_PEND2_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskPend2Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_PEND2_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_PEND2_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
				  
	/********STATUS TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskStatusTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Status",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskStatus,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_STATUS_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskStatusStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_STATUS_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_STATUS_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
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
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	char        msg[] = "��ã�����uC/OS��";  // ��Ϣ����
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      		
	   /* ������Ϣ����Ϣ���� queue */
		OSQPost ((OS_Q        *)&queue,          /* ��Ϣ����ָ�� */
                 (uint8_t     *)msg,             /* Ҫ���͵����ݵ�ָ�룬���ڴ��ַͨ�����С����ͳ�ȥ��*/
                 (OS_MSG_SIZE  )sizeof (msg),    /* �����ֽڴ�С */
                 //(OS_OPT       )OS_OPT_POST_FIFO | OS_OPT_POST_ALL, /* �Ƚ��ȳ��ͷ�����ȫ���������ʽ */
				 (OS_OPT       )OS_OPT_POST_FIFO,   /* ���Ƚ��ȳ�����ʽ */
                 (OS_ERR      *)&err);              /* ���ش������� */
				 
		if ( err == OS_ERR_NONE )  // ������ͳɹ�
		{    
			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			printf ( "\r\nAppTaskPost��������Ϣ�����ݣ�%s\r\n", msg );
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
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	OS_MSG_SIZE msg_size;
	char*       msg;
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		/* ������Ϣ���� queue ����Ϣ */
        msg = OSQPend ((OS_Q         *)&queue,     /* ��Ϣ����ָ�� */
                       (OS_TICK       )0,          /* �ȴ�ʱ��Ϊ���� */
                       (OS_OPT        )OS_OPT_PEND_BLOCKING, /* ���û�л�ȡ���ź����͵ȴ� */
                       (OS_MSG_SIZE  *)&msg_size,  /* ��ȡ��Ϣ���ֽڴ�С */
                       (CPU_TS       *)0,          /* ��ȡ������ʱ��ʱ��� */
                       (OS_ERR       *)&err);      /* ���ش��� */

		if ( err == OS_ERR_NONE )  // ������ճɹ�
		{    
			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			printf ( "\r\nAppTaskPend1��������Ϣ�ĳ��ȣ�%d�ֽڣ����ݣ�%s\r\n", msg_size, msg );
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
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	OS_MSG_SIZE msg_size;
	char*       msg;
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		/* ������Ϣ���� queue ����Ϣ */
        msg = OSQPend ((OS_Q         *)&queue,     /* ��Ϣ����ָ�� */
                       (OS_TICK       )0,          /* �ȴ�ʱ��Ϊ���� */
                       (OS_OPT        )OS_OPT_PEND_BLOCKING, /* ���û�л�ȡ���ź����͵ȴ� */
                       (OS_MSG_SIZE  *)&msg_size,  /* ��ȡ��Ϣ���ֽڴ�С */
                       (CPU_TS       *)0,          /* ��ȡ������ʱ��ʱ��� */
                       (OS_ERR       *)&err);      /* ���ش��� */

		if ( err == OS_ERR_NONE )  // ������ճɹ�
		{    
			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			printf ( "\r\nAppTaskPend2��������Ϣ�ĳ��ȣ�%d�ֽڣ����ݣ�%s\r\n", msg_size, msg );
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
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{  
		OS_CRITICAL_ENTER();
		printf("---------------------------------------------------\r\n");
        printf ( "CPU�����ʣ�%d.%d%%\r\n",
            OSStatTaskCPUUsage / 100, OSStatTaskCPUUsage % 100 );
        printf ( "CPU��������ʣ�%d.%d%%\r\n",
            OSStatTaskCPUUsageMax / 100, OSStatTaskCPUUsageMax % 100 );


        printf ( "Post�����CPU�����ʣ�%d.%d%%\r\n",
            AppTaskPostTCB.CPUUsageMax / 100, AppTaskPostTCB.CPUUsageMax % 100 );
        printf ( "Pend1�����CPU�����ʣ�%d.%d%%\r\n",
            AppTaskPend1TCB.CPUUsageMax / 100, AppTaskPend1TCB.CPUUsageMax % 100 );
        printf ( "Pend2�����CPU�����ʣ�%d.%d%%\r\n",
            AppTaskPend2TCB.CPUUsageMax / 100, AppTaskPend2TCB.CPUUsageMax % 100 );
        printf ( "Status�����CPU�����ʣ�%d.%d%%\r\n",
            AppTaskStatusTCB.CPUUsageMax / 100, AppTaskStatusTCB.CPUUsageMax % 100 ) ;


        printf ( "Post��������úͿ���ջ��С�ֱ�Ϊ��%d, %d\r\n",
                AppTaskPostTCB.StkUsed, AppTaskPostTCB.StkFree );
        printf ( "Pend1��������úͿ���ջ��С�ֱ�Ϊ��%d, %d\r\n",
                AppTaskPend1TCB.StkUsed, AppTaskPend1TCB.StkFree );
        printf ( "Pend2��������úͿ���ջ��С�ֱ�Ϊ��%d, %d\r\n",
                AppTaskPend2TCB.StkUsed, AppTaskPend2TCB.StkFree );
        printf ( "Status��������úͿ���ջ��С�ֱ�Ϊ��%d, %d\r\n",
                AppTaskStatusTCB.StkUsed, AppTaskStatusTCB.StkFree );

        printf("---------------------------------------------------\r\n");
        OS_CRITICAL_EXIT();                               

        OSTimeDlyHMSM (0, 0, 2, 0, OS_OPT_TIME_PERIODIC, &err);
	}
}
