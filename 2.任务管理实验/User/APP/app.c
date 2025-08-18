/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskLed0TCB;
static OS_TCB   AppTaskLed1TCB;
static OS_TCB	AppTaskBeepTCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskLed0Stk [APP_TASK_LED0_STK_SIZE ];
static CPU_STK   AppTaskLed1Stk [APP_TASK_LED1_STK_SIZE ];
static CPU_STK	 AppTaskBeepStk [APP_TASK_BEEP_STK_SIZE ];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskLED0  (void *p_arg);
static void AppTaskLED1  (void *p_arg);
static void AppTaskBeep  (void *p_arg);

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
	
	/********LED0 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskLed0TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task LED0",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskLED0,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_LED0_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskLed0Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_LED0_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_LED0_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
	
	/********LED1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskLed1TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task LED1",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskLED1,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_LED1_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskLed1Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_LED1_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_LED1_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
				  
	/********BEEP TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskBeepTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Beep",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskBeep,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_BEEP_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskBeepStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_BEEP_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_BEEP_STK_SIZE,		/* ����ջ��С 		*/
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
*                              LED0 TASK
***********************************************************************
*/
static  void  AppTaskLED0 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
    (void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      
		LED0_REVERSE;    // �л� LED0 ������״̬
        value = OSTaskRegGet ( 0, 0, & err );        // ��ȡ��������Ĵ���ֵ

		if ( value < 10 ) // �������Ĵ���ֵ<10
		{                          
			OSTaskRegSet ( 0, 0, ++ value, & err );    // �����ۼ�����Ĵ���
        } 
		else  // ����ۼӵ�10
		{                                     
            OSTaskRegSet ( 0, 0, 0, & err );           // ������Ĵ���ֵ��0
            printf("\r\nAppTaskLED0���һָ���LED1����\r\n");
            OSTaskResume ( & AppTaskLed1TCB, & err );  // �ָ� LED1 ����

            printf("\r\nAppTaskLED0���һָ���Beep����\r\n");
            OSTaskResume ( & AppTaskBeepTCB, & err );  // �ָ� Beep ����
        }
            OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err );  // �������ʱ1000�����ģ�1s��
    }
}

/*
***********************************************************************
*                         LED1 TASK
**********************************************************************
*/
static  void  AppTaskLED1 ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		LED1_REVERSE;    // �л� LED1 ������״̬
		value = OSTaskRegGet ( 0, 0, & err );      // ��ȡ��������Ĵ���ֵ

		if ( value < 5 ) 
		{                         // �������Ĵ���ֵ<5
			OSTaskRegSet ( 0, 0, ++ value, & err );  // �����ۼ�����Ĵ���ֵ
		} 
		else 
		{                                   // ����ۼӵ�5
			OSTaskRegSet ( 0, 0, 0, & err );         // ������Ĵ���ֵ��0
			printf("\r\nAppTaskLED1�����Լ�����LED1����\r\n");
			OSTaskSuspend ( 0, & err );              // ��������
		}
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); // �������ʱ1000��ʱ�ӽ��ģ�1s��
	}
}

/*
***********************************************************************
*                         BEEP TASK
**********************************************************************
*/
static  void  AppTaskBeep ( void * p_arg )
{
	OS_ERR      err;
	OS_REG      value;
	(void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		BEEP_REVERSE;    // �л� Beep ������״̬
		value = OSTaskRegGet ( 0, 0, & err );      // ��ȡ��������Ĵ���ֵ

		if ( value < 5 ) 
		{                         // �������Ĵ���ֵ<5
			OSTaskRegSet ( 0, 0, ++ value, & err );  // �����ۼ�����Ĵ���ֵ
		} 
		else 
		{                                   // ����ۼӵ�5
			OSTaskRegSet ( 0, 0, 0, & err );         // ������Ĵ���ֵ��0
			printf("\r\nAppTaskBeep�����Լ�����Beep����\r\n");
			OSTaskSuspend ( 0, & err );              // ��������
		}
		OSTimeDly ( 1000, OS_OPT_TIME_DLY, & err ); // �������ʱ1000��ʱ�ӽ��ģ�1s��
	}
}
