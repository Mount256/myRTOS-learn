/*
************************************************************************
*                              �������ļ�
*************************************************************************
*/
#include <includes.h>

/*
************************************************************************
*                           LOCAL  DEFINES
*************************************************************************
*/
extern uint8_t  ReceiveBuffer[RECEIVE_BUFFER_SIZE];
extern uint16_t USART1_RX_Sta; 

/*
*************************************************************************
*                                ������ƿ�TCB
*************************************************************************
*/
static OS_TCB   AppTaskStartTCB;    
static OS_TCB   AppTaskUsartTCB;
static OS_TCB   AppTaskKey0TCB;
static OS_TCB   AppTaskKey1TCB;

/*
*************************************************************************
*                                 �����ջ
*************************************************************************
*/
static CPU_STK   AppTaskStartStk[APP_TASK_START_STK_SIZE];       
static CPU_STK   AppTaskUsartStk[APP_TASK_USART_STK_SIZE];
static CPU_STK   AppTaskKey0Stk [APP_TASK_KEY0_STK_SIZE];
static CPU_STK   AppTaskKey1Stk [APP_TASK_KEY1_STK_SIZE];

/*
************************************************************************
*                           ����ԭ��
*************************************************************************
*/
static void AppTaskStart (void *p_arg);               
static void AppTaskUsart (void *p_arg);
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
	
	/********USART TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskUsartTCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Usart",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskUsart,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_USART_PRIO,		    /* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskUsartStk[0],			/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_USART_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_USART_STK_SIZE,		/* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
	
	/********KEY0 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey0TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Key0",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskKey0,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY0_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskKey0Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY0_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY0_STK_SIZE,		    /* ����ջ��С 		*/
			      (OS_MSG_QTY  	 ) 	5u,				/* ���ÿ��Է��͵�����������Ϣ�� 	*/
                  (OS_TICK       ) 	0u,				/* ������֮��ѭ��ʱ��ʱ��Ƭ��ʱ���� 	*/					
                  (void       	*) 	0,				/* ָ���û��ṩ���ڴ�λ�õ�ָ�룬���� TCB ��չ */
                  (OS_OPT        )	(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR), /* �����ض�ѡ�� */
                  (OS_ERR     	*)	&err
				);
				  
	/********KEY1 TASK CREATE*********/
	OSTaskCreate( (OS_TCB 		*) 	&AppTaskKey1TCB,  				/* ������ƿ� 		*/
				  (CPU_CHAR 	*)	"App Task Key1",				/* �������� 			*/
				  (OS_TASK_PTR	 )	AppTaskKey1,					/* ���������� 		*/
				  (void 		*)	0,								/* ������ں����β� 	*/
				  (OS_PRIO	 	 )	APP_TASK_KEY1_PRIO,				/* ��������ȼ� 		*/
				  (CPU_STK		*)	&AppTaskKey1Stk[0],				/* ջ����ʼ��ַ 		*/
				  (CPU_STK_SIZE	 )  APP_TASK_KEY1_STK_SIZE / 10, 	/* ����ջ������λ�� 	*/
				  (CPU_STK_SIZE  )	APP_TASK_KEY1_STK_SIZE,		    /* ����ջ��С 		*/
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
*                              USART TASK
***********************************************************************
*/
static  void  AppTaskUsart ( void * p_arg )
{
	OS_ERR      err;
	CPU_SR_ALLOC();
    (void)      p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{      
		OSTaskSemPend ((OS_TICK   )0,                     // �����޵ȴ�
					   (OS_OPT    )OS_OPT_PEND_BLOCKING,  // ����ź��������þ͵ȴ�
					   (CPU_TS   *)0,                     // ��ȡ�ź�����������ʱ���
					   (OS_ERR   *)&err);                 // ���ش�������

		OS_CRITICAL_ENTER();                              // �����ٽ�Σ����⴮�ڴ�ӡ�����
		printf("\r\nAppTaskUsart:������������:%s", ReceiveBuffer);
		printf("\r\nAppTaskUsart:�������ݳ���:%d\r\n", USART1_RX_Sta);
		memset(ReceiveBuffer, 0, RECEIVE_BUFFER_SIZE);    // ��ս��ջ�����
		OS_CRITICAL_EXIT();                               // �˳��ٽ��
    }
}

/*
***********************************************************************
*                         KEY0 TASK
**********************************************************************
*/
static  void  AppTaskKey0 ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	(void)         p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		OSTaskSemPend ((OS_TICK   )0,                     // �����޵ȴ�
					   (OS_OPT    )OS_OPT_PEND_BLOCKING,  // ����ź��������þ͵ȴ�
					   (CPU_TS   *)0,                     // ��ȡ�ź�����������ʱ���
					   (OS_ERR   *)&err);                 // ���ش�������
		
		LED0_REVERSE;
		ts_int = CPU_IntDisMeasMaxGet ();                 // ��ȡ�����ж�ʱ��

		OS_CRITICAL_ENTER();                              // �����ٽ�Σ����⴮�ڴ�ӡ�����
		printf ("\r\nAppTaskKey0:���������ж�,����ж�ʱ����%dus\r\n",  ts_int / (cpu_clk_freq/1000000));        			
		OS_CRITICAL_EXIT();                               // �˳��ٽ��
	}
}

/*
***********************************************************************
*                         KEY1 TASK
**********************************************************************
*/
static  void  AppTaskKey1 ( void * p_arg )
{
	OS_ERR         err;
	CPU_TS_TMR     ts_int;
	CPU_INT32U     cpu_clk_freq;
	CPU_SR_ALLOC();
	(void)         p_arg;

    while (DEF_TRUE) // �����壬ͨ��д��һ����ѭ��
	{   
		OSTaskSemPend ((OS_TICK   )0,                     // �����޵ȴ�
					   (OS_OPT    )OS_OPT_PEND_BLOCKING,  // ����ź��������þ͵ȴ�
					   (CPU_TS   *)0,                     // ��ȡ�ź�����������ʱ���
					   (OS_ERR   *)&err);                 // ���ش�������
		
		LED1_REVERSE;
		ts_int = CPU_IntDisMeasMaxGet ();                 // ��ȡ�����ж�ʱ��

		OS_CRITICAL_ENTER();                              // �����ٽ�Σ����⴮�ڴ�ӡ�����
		printf ("\r\nAppTaskKey1:���������ж�,����ж�ʱ����%dus\r\n",  ts_int / (cpu_clk_freq/1000000));        			
		OS_CRITICAL_EXIT();                               // �˳��ٽ��
	}
}

/*
************************************************************************
*                       INTERRUPT  HANDLER
***********************************************************************
*/
void EXTI3_IRQHandler(void)
{
	OS_ERR  err;
	
	OSIntEnter();  // �����ж�
	
	if (GPIO_ReadInputDataBit(KEY1) == KEY_ON)
	{
		while (GPIO_ReadInputDataBit(KEY1) == KEY_ON);  // �ȴ��������ɿ�
		/* ���������ź��������� AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskKey1TCB,   // Ŀ������
					  (OS_OPT   )OS_OPT_POST_NONE,  // ûѡ��Ҫ��
					  (OS_ERR  *)&err);             // ���ش�������		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line3);
	OSIntExit();   // �˳��ж�
}


void EXTI4_IRQHandler(void)
{
	OS_ERR  err;
	
	OSIntEnter();  // �����ж�
	
	if (GPIO_ReadInputDataBit(KEY0) == KEY_ON)
	{
		while (GPIO_ReadInputDataBit(KEY0) == KEY_ON);  // �ȴ��������ɿ�
		/* ���������ź��������� AppTaskKey */
		OSTaskSemPost((OS_TCB  *)&AppTaskKey0TCB,   // Ŀ������
					  (OS_OPT   )OS_OPT_POST_NONE,  // ûѡ��Ҫ��
					  (OS_ERR  *)&err);             // ���ش�������
	}
	
	EXTI_ClearITPendingBit(EXTI_Line4);
	OSIntExit();   // �˳��ж�
}


void USART1_IRQHandler(void)
{
	OS_ERR  err;
	
	OSIntEnter();  // �����ж�
 
    // �ж��Ƿ�Ϊ�����ж�
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) 
	{
		// ���㱾�ν��յ������ݳ���
        USART1_RX_Sta = USART_Get_RX_Len(USART1_DMA_RX_Channel); // �����ȴ���״̬����
		
        DMA_Cmd(USART1_DMA_RX_Channel, DISABLE);
        USART_ReceiveData(USART1);     // ��� USART �Ŀ����жϱ�־λ��ͨ�������ݼĴ�����
        DMA_ClearFlag(DMA1_FLAG_TC5);  // ��� DMA �ı�־λ
        USART1_DMA_RX_Channel->CNDTR = RECEIVE_BUFFER_SIZE;  // ���¸�ֵ����ֵ
        DMA_Cmd(USART1_DMA_RX_Channel, ENABLE);
 
		// �����ź��������ͽ��յ������ݱ�־����ǰ̨�����ѯ
        OSTaskSemPost((OS_TCB  *)&AppTaskUsartTCB,   // Ŀ������
					  (OS_OPT   )OS_OPT_POST_NONE,   // ûѡ��Ҫ��
					  (OS_ERR  *)&err);              // ���ش�������
    }
	
	OSIntExit();   // �˳��ж�
}
