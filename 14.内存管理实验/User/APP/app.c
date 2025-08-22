/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
*************************************************************************
*                            LOCAL DEFINES
*************************************************************************
*/
OS_MEM  mem;                    // �����ڴ�������
uint8_t ucArray [5] [20];       // �����ڴ������С

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
static CPU_STK   AppTaskPost1Stk  [APP_TASK_POST1_STK_SIZE ];
static CPU_STK   AppTaskPost2Stk  [APP_TASK_POST2_STK_SIZE ];
static CPU_STK   AppTaskPendStk [APP_TASK_PEND_STK_SIZE];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost1  (void *p_arg);
static void AppTaskPost2  (void *p_arg);
static void AppTaskPend (void *p_arg);

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

	/* �����ڴ������� mem */
	OSMemCreate ((OS_MEM      *)&mem,             // ָ���ڴ�������
				 (CPU_CHAR    *)"Mem For Test",   // �����ڴ�������
				 (void        *)ucArray,          // �ڴ�������׵�ַ
				 (OS_MEM_QTY   )5,                // �ڴ�������ڴ����ĿΪ5
				 (OS_MEM_SIZE  )20,               // �ڴ����ֽ���ĿΪ20
				 (OS_ERR      *)&err);            // ���ش�������
	
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
				  (CPU_STK_SIZE  )	APP_TASK_PEND_STK_SIZE,		/* ����ջ��С 		*/
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
	char *      p_mem_blk;
	CPU_SR_ALLOC();
	uint32_t    ulCount = 0;
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      		
	    p_mem_blk = OSMemGet ((OS_MEM      *)&mem,             // ָ���ڴ�������
		                      (OS_ERR      *)&err);            // ���ش�������
		
		OS_CRITICAL_ENTER();                              // �����ٽ�Σ����⴮�ڴ�ӡ�����
		printf ("\r\nAppTaskPost1�������ڴ�飡\r\n");
		OS_CRITICAL_EXIT();                               // �˳��ٽ��
		
		sprintf (p_mem_blk, "%d", ulCount++);                  // ���ڴ���ȡ����ֵ
		
		/* ����������Ϣ������ AppTaskPend */
		OSTaskQPost ((OS_TCB      *)&AppTaskPendTCB,           // Ŀ������Ŀ��ƿ�
					 (void        *)p_mem_blk,                 // ��Ϣ���ݵ��׵�ַ
					 (OS_MSG_SIZE  )strlen ( p_mem_blk ),      // ��Ϣ����
					 (OS_OPT       )OS_OPT_POST_FIFO,          // ������������Ϣ���е���ڶ�
					 (OS_ERR      *)&err);                     // ���ش�������
				
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

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      		
	    p_mem_blk = OSMemGet ((OS_MEM      *)&mem,             // ָ���ڴ�������
		                      (OS_ERR      *)&err);            // ���ش�������
		
		OS_CRITICAL_ENTER();                              // �����ٽ�Σ����⴮�ڴ�ӡ�����
		printf ("\r\nAppTaskPost2�������ڴ�飡\r\n");
		OS_CRITICAL_EXIT();                               // �˳��ٽ��
		
		sprintf (p_mem_blk, "%d", ulCount++);                  // ���ڴ���ȡ����ֵ
		
		/* ����������Ϣ������ AppTaskPend */
		OSTaskQPost ((OS_TCB      *)&AppTaskPendTCB,           // Ŀ������Ŀ��ƿ�
					 (void        *)p_mem_blk,                 // ��Ϣ���ݵ��׵�ַ
					 (OS_MSG_SIZE  )strlen ( p_mem_blk ),      // ��Ϣ����
					 (OS_OPT       )OS_OPT_POST_FIFO,          // ������������Ϣ���е���ڶ�
					 (OS_ERR      *)&err);                     // ���ش�������
				
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

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		/* �������񣬵ȴ�������Ϣ */
		pMsg = OSTaskQPend ((OS_TICK        )0,                    // �����޵ȴ�
							(OS_OPT         )OS_OPT_PEND_BLOCKING, // û����Ϣ����������
							(OS_MSG_SIZE   *)&msg_size,            // ������Ϣ����
							(CPU_TS        *)0,                    // ������Ϣ��������ʱ���������Ҫ��
							(OS_ERR        *)&err);                // ���ش�������
		
		LED1_REVERSE;                               
		
		OS_CRITICAL_ENTER();                              // �����ٽ�Σ����⴮�ڴ�ӡ�����
		printf ( "\r\n���յ�����Ϣ������Ϊ��%s�������ǣ�%d�ֽڡ�", pMsg, msg_size );   
		printf ( "\r\nAppTaskPend�ͷ��ڴ�飡\r\n" ); 		
		OS_CRITICAL_EXIT();                               // �˳��ٽ��
		
		/* �˻��ڴ�� */
		OSMemPut ((OS_MEM  *)&mem,                        // ָ���ڴ�������
				  (void    *)pMsg,                        // �ڴ����׵�ַ
				  (OS_ERR  *)&err);		                  // ���ش�������
	}
}
