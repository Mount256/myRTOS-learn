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
static OS_TCB   AppTaskPost1TCB;
static OS_TCB   AppTaskPost2TCB;
static OS_TCB   AppTaskPendTCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk [APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskPost1Stk [APP_TASK_POST1_STK_SIZE];
static CPU_STK   AppTaskPost2Stk [APP_TASK_POST2_STK_SIZE];
static CPU_STK   AppTaskPendStk  [APP_TASK_PEND_STK_SIZE];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost1 (void *p_arg);
static void AppTaskPost2 (void *p_arg);
static void AppTaskPend  (void *p_arg);

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
	
	/********POST1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPost1TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Post 1",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskPost1,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_POST1_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskPost1Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_POST1_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_POST1_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
	
	/********POST2 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPost2TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Post 2",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskPost2,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_POST2_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskPost2Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_POST2_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_POST2_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
				  
	/********PEND TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskPendTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Pend",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskPend,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_PEND_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskPendStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_PEND_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_PEND_STK_SIZE,		    /* ����ջ��С 		*/
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
*                              POST1 TASK
***********************************************************************
*/
static  void  AppTaskPost1 ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	char        msg[] = "��ã�����POST1��";  // ��Ϣ����
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      		
		OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
		printf ( "\r\nAppTaskPost1��������Ϣ�����ݣ�%s\r\n", msg );
		OS_CRITICAL_EXIT();
		
	   /* ������Ϣ����Ϣ���� queue */
		OSQPost ((OS_Q        *)&queue,          /* ��Ϣ����ָ�� */
                 (uint8_t     *)msg,             /* Ҫ���͵����ݵ�ָ�룬���ڴ��ַͨ�����С����ͳ�ȥ��*/
                 (OS_MSG_SIZE  )sizeof (msg),    /* �����ֽڴ�С */
                 //(OS_OPT       )OS_OPT_POST_LIFO, /* ������ȳ�����ʽ */
				 (OS_OPT       )OS_OPT_POST_FIFO,   /* ���Ƚ��ȳ�����ʽ */
                 (OS_ERR      *)&err);              /* ���ش������� */
				 
		if ( err == OS_ERR_NONE )  // ������ͳɹ�
		{    
			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			printf ( "\r\nAppTaskPost1���ѳɹ�������Ϣ��\r\n" );
			OS_CRITICAL_EXIT();
        }
				
        OSTimeDly ( 4000, OS_OPT_TIME_DLY, &err ); 
    }
}

/*
************************************************************************
*                              POST2 TASK
***********************************************************************
*/
static  void  AppTaskPost2 ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	char        msg[] = "��ã�����POST2��";  // ��Ϣ����
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      		
		OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
		printf ( "\r\nAppTaskPost2��������Ϣ�����ݣ�%s\r\n", msg );
		OS_CRITICAL_EXIT();
		
	   /* ������Ϣ����Ϣ���� queue */
		OSQPost ((OS_Q        *)&queue,          /* ��Ϣ����ָ�� */
                 (uint8_t     *)msg,             /* Ҫ���͵����ݵ�ָ�룬���ڴ��ַͨ�����С����ͳ�ȥ��*/
                 (OS_MSG_SIZE  )sizeof (msg),    /* �����ֽڴ�С */
                 //(OS_OPT       )OS_OPT_POST_LIFO, /* ������ȳ�����ʽ */
				 (OS_OPT       )OS_OPT_POST_FIFO,   /* ���Ƚ��ȳ�����ʽ */
                 (OS_ERR      *)&err);              /* ���ش������� */
				 
		if ( err == OS_ERR_NONE )  // ������ͳɹ�
		{    
			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			printf ( "\r\nAppTaskPost2���ѳɹ�������Ϣ��\r\n" );
			OS_CRITICAL_EXIT();
        }
				
        OSTimeDly ( 8000, OS_OPT_TIME_DLY, &err ); 
    }
}

/*
***********************************************************************
*                         Pend TASK
**********************************************************************
*/
static  void  AppTaskPend ( void * p_arg )
{
	OS_ERR      err;
	
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	OS_MSG_SIZE msg_size;
	char*       msg;
	
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
		printf ( "\r\nAppTaskPend��׼����ȡ��Ϣ...\r\n" );
		OS_CRITICAL_EXIT();
		
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
			printf ( "\r\nAppTaskPend��������Ϣ�ĳ��ȣ�%d�ֽڣ����ݣ�%s\r\n", msg_size, msg );
			OS_CRITICAL_EXIT();
        }
	}
}

