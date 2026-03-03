// Key.c - 适配"IO口-开关-3.3V"电路
#include "stm32f10x.h"
#include "Delay.h"
#include "Buzzer.h"

void Key_Init(void)
{
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  // 下拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // 按键1 - PA12
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    Delay_ms(10);
}

// 重要：按键按下时为高电平(1)，释放时为浮空(不确定)
// 需要在硬件上添加外部下拉电阻，或者在软件中特殊处理
uint8_t Key_GetNum(void)
{
    // 直接检测高电平（按键按下）
    
    // 按键1 - 按下时为高电平
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1)
    {
        Delay_ms(20);  // 消抖
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == 1)
        {
            // 等待按键释放
            Delay_ms(20);
            BUZZER_50ms();
            return 1;
        }
    }
    
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1)
    {
        Delay_ms(20);  // 消抖
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5) == 1)
        {
            // 等待按键释放
            Delay_ms(20);
            BUZZER_50ms();
            return 2;
        }
    }

        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 1)
    {
        Delay_ms(20);  // 消抖
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)== 1)
        {
            // 等待按键释放
            Delay_ms(20);
            BUZZER_50ms();
            return 3;
        }
    }
    
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1)
    {
        Delay_ms(20);  // 消抖
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7) == 1)
        {
            // 等待按键释放
            Delay_ms(20);
            BUZZER_50ms();
            return 4;
        }
    }
    
    
    
    return 0;

}
