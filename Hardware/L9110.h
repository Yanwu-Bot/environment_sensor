#ifndef __L9110_H
#define __L9110_H

#include "stm32f10x.h"

/* 引脚定义 */
#define FAN_INA_PIN     GPIO_Pin_3
#define FAN_INA_PORT    GPIOA
#define FAN_INA_CLK     RCC_APB2Periph_GPIOA

#define FAN_INB_PIN     GPIO_Pin_0
#define FAN_INB_PORT    GPIOB
#define FAN_INB_CLK     RCC_APB2Periph_GPIOB

/* 风扇状态枚举 */
typedef enum {
    FAN_STOP = 0,      // 停止
    FAN_FORWARD,       // 正转（吹风）
    FAN_REVERSE,       // 反转（抽风）
    FAN_BRAKE          // 刹车
} FanState_TypeDef;

/* 函数声明 */
void Fan_Init(void);
void Fan_Control(FanState_TypeDef state);
void Fan_Forward(void);
void Fan_Reverse(void);
void Fan_Stop(void);
void Fan_Brake(void);
FanState_TypeDef Fan_GetState(void);
void Fan_Toggle(void);  // 在正转和停止之间切换

#endif /* __FAN_H */
