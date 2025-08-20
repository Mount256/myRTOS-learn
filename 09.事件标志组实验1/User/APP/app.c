/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
OS_FLAG_GRP flag_grp;            // �����¼���־��

#define KEY0_EVENT  (0x01 << 0)  // �����¼������λ0
#define KEY1_EVENT  (0x01 << 1)  // �����¼������λ1

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskPostTCB;
static OS_TCB   AppTaskPendTCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskPostStk [APP_TASK_POST_STK_SIZE ];
static CPU_STK   AppTaskPendStk [APP_TASK_PEND_STK_SIZE ];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskPost  (void *p_arg);
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

	 /* �����¼���־�� flag_grp */
	OSFlagCreate ((OS_FLAG_GRP  *)&flag_grp,        // ָ���¼���־���ָ��
                  (CPU_CHAR     *)"FLAG For Test",  // �¼���־�������
                  (OS_FLAGS      )0,                // �¼���־��ĳ�ʼֵ
                  (OS_ERR       *)&err);			// ���ش�������
	
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
*                              POST TASK
***********************************************************************
*/
static  void  AppTaskPost ( void * p_arg )
{
	OS_ERR      err;
    (void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      
		if( Key_Scan(KEY0) == KEY_ON ) // ���KEY0������
		{		                                                   
			printf("\r\nKEY0������\r\n");
			OSFlagPost ((OS_FLAG_GRP  *)&flag_grp,                             
                        (OS_FLAGS      )KEY0_EVENT,
                        (OS_OPT        )OS_OPT_POST_FLAG_SET,  // ����־���BIT0��1
                        (OS_ERR       *)&err);

		}

		if( Key_Scan(KEY1) == KEY_ON ) // ���KEY1������
		{		                                                   
			printf("\r\nKEY1������\r\n");
			OSFlagPost ((OS_FLAG_GRP  *)&flag_grp,                             
                        (OS_FLAGS      )KEY1_EVENT,
                        (OS_OPT        )OS_OPT_POST_FLAG_SET,  // ����־���BIT1��1
                        (OS_ERR       *)&err);

		}

		OSTimeDlyHMSM ( 0, 0, 0, 20, OS_OPT_TIME_DLY, & err );  // ÿ20msɨ��һ��
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

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		// �ȴ���־��ĵ�BIT0��BIT1������1 
		flags_rdy = OSFlagPend ((OS_FLAG_GRP *)&flag_grp,                 
                                (OS_FLAGS     )( KEY0_EVENT | KEY1_EVENT ),
                                (OS_TICK      )0,
                                (OS_OPT       )OS_OPT_PEND_FLAG_SET_ALL |  // Ҫ�����еȴ���־λ��Ҫ��1
		                                       OS_OPT_PEND_BLOCKING |
                                               OS_OPT_PEND_FLAG_CONSUME,   // �¼��ȴ��ɹ��󣬱���ʹ�ø�ѡ����ʽ����ѽ��յ����¼�����
                                (CPU_TS      *)0,
                                (OS_ERR      *)&err);
		
		if ( (flags_rdy & (KEY0_EVENT|KEY1_EVENT)) == (KEY0_EVENT|KEY1_EVENT) ) 
		{
			/* ���������ɲ�����ȷ */
			printf ("\r\nKEY0��KEY1������\r\n");		
			LED0_REVERSE;       
		}
		
	}
}
