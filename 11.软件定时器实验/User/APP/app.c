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

CPU_TS             ts_start;       // ʱ�������
CPU_TS             ts_end; 

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskTmrTCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskTmrStk  [ APP_TASK_TMR_STK_SIZE ];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void  AppTaskTmr  (void *p_arg);

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
	
	/********TMR TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskTmrTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Timer",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskTmr,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_TMR_STK_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskTmrStk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_TMR_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_TMR_STK_SIZE,		/* ����ջ��С 		*/
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
*                          TIMER TASK
***********************************************************************
*/
static  void  AppTaskTmr  ( void * p_arg )
{
	OS_ERR      err;
	OS_TMR      my_tmr;
    (void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      
		
    }
}


