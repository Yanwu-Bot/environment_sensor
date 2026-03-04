// L9110.c - 修改版（适配电机线接反的情况）
#include "L9110.h"
#include "stm32f10x.h"

// 引脚定义
#define FAN_INA_PIN     GPIO_Pin_3
#define FAN_INA_PORT    GPIOA
#define FAN_INB_PIN     GPIO_Pin_1
#define FAN_INB_PORT    GPIOA

void Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置INA (PA3) 和 INB (PA1)
    GPIO_InitStructure.GPIO_Pin = FAN_INA_PIN | FAN_INB_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始状态：停止
    Fan_Stop();
}

// L9110控制逻辑：
// INA=1, INB=0 → 正转
// INA=0, INB=1 → 反转
// INA=0, INB=0 → 停止
// INA=1, INB=1 → 刹车

// 修改版：交换正转和反转的定义
void Fan_Forward(void)  // 正转（开风扇）- 实际对应反转
{
    GPIO_ResetBits(FAN_INA_PORT, FAN_INA_PIN); // INA=0 (PA3)
    GPIO_SetBits(FAN_INB_PORT, FAN_INB_PIN);   // INB=1 (PA1)
}

void Fan_Reverse(void)  // 反转 - 实际对应正转
{
    GPIO_SetBits(FAN_INA_PORT, FAN_INA_PIN);   // INA=1 (PA3)
    GPIO_ResetBits(FAN_INB_PORT, FAN_INB_PIN); // INB=0 (PA1)
}

void Fan_Stop(void)  // 停止（关风扇）
{
    GPIO_ResetBits(FAN_INA_PORT, FAN_INA_PIN); // INA=0 (PA3)
    GPIO_ResetBits(FAN_INB_PORT, FAN_INB_PIN); // INB=0 (PA1)
}

void Fan_Brake(void)  // 刹车
{
    GPIO_SetBits(FAN_INA_PORT, FAN_INA_PIN);   // INA=1 (PA3)
    GPIO_SetBits(FAN_INB_PORT, FAN_INB_PIN);   // INB=1 (PA1)
}

FanState_TypeDef Fan_GetState(void)
{
    uint8_t ina = GPIO_ReadOutputDataBit(FAN_INA_PORT, FAN_INA_PIN);
    uint8_t inb = GPIO_ReadOutputDataBit(FAN_INB_PORT, FAN_INB_PIN);
    
    // 注意：这里的状态判断也要相应调整
    if(ina == 0 && inb == 0) return FAN_STOP;
    if(ina == 0 && inb == 1) return FAN_FORWARD;  // 修改这里
    if(ina == 1 && inb == 0) return FAN_REVERSE;  // 修改这里
    if(ina == 1 && inb == 1) return FAN_BRAKE;
    
    return FAN_STOP;
}