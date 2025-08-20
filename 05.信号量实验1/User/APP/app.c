/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
**************************************************************************
*                          LOCAL DEFINES
**************************************************************************
*/
OS_SEM SemOfKey;          // ��־KEY1�Ƿ񱻵����Ķ�ֵ�ź���

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskKey0TCB;
static OS_TCB   AppTaskKey1TCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskKey0Stk [APP_TASK_KEY0_STK_SIZE ];
static CPU_STK   AppTaskKey1Stk [APP_TASK_KEY1_STK_SIZE ];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskKey0  (void *p_arg);
static void AppTaskKey1  (void *p_arg);

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

	/* ������ֵ�ź��� SemOfKey */
    OSSemCreate((OS_SEM      *)&SemOfKey,  /* ָ���ź���������ָ�� */
                (CPU_CHAR    *)"SemOfKey", /* �ź���������         */
                (OS_SEM_CTR   )5,          /* ��ʾ������Դ��Ŀ     */ 
                (OS_ERR      *)&err);      /* ��������             */
	
	/********KEY0 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey0TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task KEY0",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskKey0,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY0_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskKey0Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY0_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY0_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
	
	/********KEY1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey1TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task KEY1",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskKey1,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY1_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskKey1Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY1_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY1_STK_SIZE,		/* ����ջ��С 		*/
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
*                              KEY0 TASK
***********************************************************************
*/
static  void  AppTaskKey0 ( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	OS_SEM_CTR  ctr;
    (void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      
		if ( Key_Scan(KEY0) == KEY_ON )  // ���KEY0������
		{ 
			ctr = OSSemPend ((OS_SEM   *)&SemOfKey,// �ȴ����ź��� SemOfKey
                             (OS_TICK   )0,        // ����ѡ�񲻵ȴ����ò���Ч
                             (OS_OPT    )OS_OPT_PEND_NON_BLOCKING, // ���û���ź��������򲻵ȴ�
                             (CPU_TS   *)0,        // ����ȡʱ���
                             (OS_ERR   *)&err);    // ���ش�������

			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			if ( err == OS_ERR_NONE )
				printf ( "\r\nKEY0�����£��ɹ����뵽һ��ͣ��λ��ʣ��%d��ͣ��λ��\r\n", ctr );
			else if ( err == OS_ERR_PEND_WOULD_BLOCK )
				printf ( "\r\nKEY0�����£�������˼������ͣ������������ȴ���\r\n" );
			OS_CRITICAL_EXIT();
		}

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, &err ); // ÿ20msɨ��һ��
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
	CPU_SR_ALLOC();  // ��Ҫ�����ٽ��ʱ���붨�� cpu_sr
	OS_SEM_CTR  ctr;
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		if ( Key_Scan(KEY1) == KEY_ON ) // ���KEY1������
		{ 
			ctr = OSSemPost((OS_SEM  *)&SemOfKey,       // ���� SemOfKey
                            (OS_OPT   )OS_OPT_POST_ALL, // ���������еȴ�����
                            (OS_ERR  *)&err);           // ���ش�������

			OS_CRITICAL_ENTER();   // �����ٽ�Σ���ϣ����ӡ���ڱ����
			if ( err == OS_ERR_NONE )
				printf ( "\r\nKEY1�����£��ͷ�һ��ͣ��λ��ʣ��%d��ͣ��λ��\r\n", ctr );
			OS_CRITICAL_EXIT();
		}

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, &err ); // ÿ20msɨ��һ��
	}
}

