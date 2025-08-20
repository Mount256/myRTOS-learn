/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
*******************************************************************
*                                            LOCAL DEFINES
*******************************************************************
*/

OS_MUTEX TestMutex;      // ������   

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskHighTCB;
static OS_TCB   AppTaskMedTCB;
static OS_TCB	AppTaskLowTCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskHighStk [APP_TASK_HIGH_STK_SIZE ];
static CPU_STK   AppTaskMedStk [APP_TASK_MED_STK_SIZE ];
static CPU_STK	 AppTaskLowStk [APP_TASK_LOW_STK_SIZE ];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskHigh  (void *p_arg);
static void AppTaskMed  (void *p_arg);
static void AppTaskLow  (void *p_arg);

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

	/* ���������ź��� Mutex */
	OSMutexCreate ((OS_MUTEX  *)&TestMutex,       // ָ���ź���������ָ��
                   (CPU_CHAR  *)"Mutex For Test", // �ź���������
                   (OS_ERR    *)&err);            // ��������
	
	/********HIGH TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskHighTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task High",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskHigh,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_HIGH_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskHighStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_HIGH_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_HIGH_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
	
	/********MED TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskMedTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Med",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskMed,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_MED_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskMedStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_MED_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_MED_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
				  
	/********LOW TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskLowTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Low",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskLow,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_LOW_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskLowStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_LOW_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_LOW_STK_SIZE,		/* ����ջ��С 		*/
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
*                              HIGH TASK
***********************************************************************
*/
static  void  AppTaskHigh ( void * p_arg )
{
	OS_ERR      err;
	//static uint32_t  i;
	//CPU_TS      ts_sem_post;
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		// ��ȡ����������δ��ȡ����һֱ�ȴ�
		printf("\r\nAppTaskHigh ׼����ȡ��������\r\n");
		OSMutexPend ((OS_MUTEX *)&TestMutex,  //�ȴ��û�����������
                     (OS_TICK   )0,           //�����޵ȴ�
                     (OS_OPT    )OS_OPT_PEND_BLOCKING,  // ���û���ź������þ͵ȵ�
                     (CPU_TS   *)0,           // ����Ҫʱ���
                     (OS_ERR   *)&err);       // ���ش�������
		
		printf("\r\nAppTaskHigh �ѻ�ȡ�����������������С�\r\n");
		//for (i=0; i<3000000; i++);
		//OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err); 
		delay_ms(1000);	

		// ����������
		printf("\r\nAppTaskHigh �ͷ��˻�����!\r\n");
        OSMutexPost((OS_MUTEX*)&TestMutex,
                    (OS_OPT   )OS_OPT_POST_NONE, // �����������
                    (OS_ERR  *)&err);
		
        OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err);  // ��ʱ 1s
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

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		printf("\r\nAppTaskMed ���������С�\r\n");
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

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		// ��ȡ����������δ��ȡ����һֱ�ȴ�
		printf("\r\nAppTaskLow ׼����ȡ��������\r\n");
		OSMutexPend ((OS_MUTEX *)&TestMutex,  //�ȴ��û�����������
                     (OS_TICK   )0,           //�����޵ȴ�
                     (OS_OPT    )OS_OPT_PEND_BLOCKING,  // ���û���ź������þ͵ȵ�
                     (CPU_TS   *)0,           // ����Ҫʱ���
                     (OS_ERR   *)&err);       // ���ش�������
		
		printf("\r\nAppTaskLow �ѻ�ȡ�����������������С�\r\n");
		//for (i=0; i<3000000; i++);
		//OSTimeDlyHMSM (0, 0, 3, 0, OS_OPT_TIME_PERIODIC, &err);  
		delay_ms(3000);
		
		// ����������
		printf("\r\nAppTaskLow �ͷ��˻�����!\r\n");
        OSMutexPost((OS_MUTEX*)&TestMutex,
                    (OS_OPT   )OS_OPT_POST_NONE, // �����������
                    (OS_ERR  *)&err);

        OSTimeDlyHMSM (0, 0, 1, 0, OS_OPT_TIME_PERIODIC, &err);  // ��ʱ 1s
	}
}
