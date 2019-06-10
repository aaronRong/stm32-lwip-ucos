#include "includes.h"

/*
*********************************************************************************************************
*                                        ����ջ
*********************************************************************************************************
*/
static  OS_STK         App_TaskToggleLEDStk[APP_CFG_TASK_ToggleLED_STK_SIZE];
static  OS_STK         App_TaskToggleLED1Stk[APP_CFG_TASK_ToggleLED_STK_SIZE];
/*
*********************************************************************************************************
*                                        �������
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       ����ԭ������
*********************************************************************************************************
*/
static void App_TaskToggleLED(void *p_arg);
static void App_TaskToggleLED1(void *p_arg);

/*
 * ��������App_TaskCreate
 * ��  ����
 * ��  �룺��
 * ��  ������
 */
void App_TaskCreate(void)
{
#if OS_TASK_NAME_SIZE > 7
    INT8U  err;
#endif

    //LED ����˸����
#if OS_TASK_CREATE_EXT_EN > 0
    OSTaskCreateExt((void (*)(void *)) App_TaskToggleLED,             /* Create the start task                                    */
                    (void           *) 0,
                    (OS_STK         *)&App_TaskToggleLEDStk[APP_CFG_TASK_ToggleLED_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_ToggleLED_PRIO,
                    (INT16U          ) APP_CFG_TASK_ToggleLED_PRIO,
                    (OS_STK         *)&App_TaskToggleLEDStk[0],
                    (INT32U          ) APP_CFG_TASK_ToggleLED_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	
	OSTaskCreate((void (*)(void *)) App_TaskToggleLED1,
           (void          * ) 0,
           (OS_STK        * )&App_TaskToggleLED1Stk[APP_CFG_TASK_ToggleLED_STK_SIZE - 1],
           (INT8U           ) APP_CFG_TASK_ToggleLED1_PRIO
            );
#else    
    OSTaskCreate((void (*)(void *)) App_TaskToggleLED,
           (void          * ) 0,
           (OS_STK        * )&App_TaskToggleLEDStk[APP_CFG_TASK_ToggleLED_STK_SIZE - 1],
           (INT8U           ) APP_CFG_TASK_ToggleLED_PRIO
            );

    
#endif    
   
}

/*
 * ��������App_TaskToggleLED
 * ��  ����
 * ��  �룺��
 * ��  ������
 */
static void App_TaskToggleLED(void *p_arg)
{
  (void)p_arg;
	
  while(1)
  {
    BSP_LedToggle();
    OSTimeDlyHMSM(0, 0, 1, 0);
  }
}

static void App_TaskToggleLED1(void *p_arg)
{
  (void)p_arg;
	
  while(1)
  {
    BSP_Led1Toggle();
    OSTimeDlyHMSM(0, 0, 0, 500);
  }
}

