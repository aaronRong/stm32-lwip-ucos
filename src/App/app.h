#ifndef _APP_H__
#define _APP_H__

/* macros */
#define PIN_RELAY_ONE				(GPIO_Pin_11)
#define PIN_RELAY_TWO				(GPIO_Pin_12)
#define PIN_RELAY_THREE				(GPIO_Pin_13)

typedef enum _RelayCon
{
	RELAY_ONE_ON = 0x00,
	RELAY_ONE_OFF,
	RELAY_TWO_ON,
	RELAY_TWO_OFF,
	RELAY_THREE_ON,
	RELAY_THREE_OFF,
	RELAY_CON_MAX,
}RelayCon;

/*
*********************************************************************************************************
*                                       外部函数声明
*********************************************************************************************************
*/
void App_TaskCreate(void);

#endif //_APP_H__
