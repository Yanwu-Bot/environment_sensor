#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

/* 初始化 */
void LED_Init(void);
void LED_All_Off(void);

/* PA口系统状态灯 */
void LEDR_ON(void);
void LEDR_OFF(void);
void LEDR_Toggle(void);

void LEDB_ON(void);
void LEDB_OFF(void);
void LEDB_Toggle(void);

void LEDG_ON(void);
void LEDG_OFF(void);
void LEDG_Toggle(void);

/* PB口传感器状态灯 */
void LED_AHT20_Normal_ON(void);
void LED_AHT20_Normal_OFF(void);
void LED_AHT20_Warn_ON(void);
void LED_AHT20_Warn_OFF(void);
void LED_AHT20_Error_ON(void);
void LED_AHT20_Error_OFF(void);

void LED_MQ2_Normal_ON(void);
void LED_MQ2_Normal_OFF(void);

/* 批量控制 */
void LED_AHT20_SetLevel(uint8_t level);
void LED_MQ2_SetLevel(uint8_t level);

#endif
