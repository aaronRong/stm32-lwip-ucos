#ifndef _BSP_H__
#define _BSP_H__

#include "includes.h"
/*
*********************************************************************************************************
*                                       外部函数声明
*********************************************************************************************************
*/
void BSP_Init(void);
void BSP_RelayInit(void);
void BSP_Relay_ON(uint16_t pin_id);
void BSP_Relay_OFF(uint16_t pin_id);
void BSP_LedToggle(void);
void BSP_Led1Toggle (void);
void SysTick_Init(void);
uint8_t BSP_Relay_Con(uint8_t relayVal);

#endif //_BSP_H__
