#include "stm32f10x.h"                  // Device header

/**
 * @brief   LED初始化
 * @note    PA8: 红灯
 *          PA9: 蓝灯
 *          PA10: 绿灯
 *          PB12: AHT20正常指示灯
 *          PB13: AHT20警告指示灯
 *          PB14: AHT20异常指示灯
 *          PB15: MQ2正常指示灯
 */
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOA和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置PA8、PA9、PA10为推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置PB12、PB13、PB14、PB15为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 初始状态：所有LED熄灭（假设低电平点亮，所以初始给高电平）
    GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

/* ========== PA口LED控制（系统状态灯） ========== */

// 红灯 (PA8)
void LEDR_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);  // 低电平点亮
}

void LEDR_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_8);    // 高电平熄灭
}

void LEDR_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8) == 0) {
        GPIO_SetBits(GPIOA, GPIO_Pin_8);
    } else {
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    }
}

// 蓝灯 (PA9)
void LEDB_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
}

void LEDB_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_9);
}

void LEDB_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_9) == 0) {
        GPIO_SetBits(GPIOA, GPIO_Pin_9);
    } else {
        GPIO_ResetBits(GPIOA, GPIO_Pin_9);
    }
}

// 绿灯 (PA10)
void LEDG_ON(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_10);
}

void LEDG_OFF(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_10);
}

void LEDG_Toggle(void)
{
    if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_10) == 0) {
        GPIO_SetBits(GPIOA, GPIO_Pin_10);
    } else {
        GPIO_ResetBits(GPIOA, GPIO_Pin_10);
    }
}

/* ========== PB口LED控制（传感器状态灯） ========== */

// AHT20正常指示灯 (PB12)
void LED_AHT20_Normal_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}

void LED_AHT20_Normal_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

// AHT20警告指示灯 (PB13)
void LED_AHT20_Warn_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

void LED_AHT20_Warn_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
}

// AHT20异常指示灯 (PB14)
void LED_AHT20_Error_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);
}

void LED_AHT20_Error_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

// MQ2正常指示灯 (PB15)
void LED_MQ2_Normal_ON(void)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void LED_MQ2_Normal_OFF(void)
{
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

/* ========== 批量控制函数 ========== */

/**
 * @brief   关闭所有LED
 */
void LED_All_Off(void)
{
    // 关闭PA口所有LED
    GPIO_SetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
    
    // 关闭PB口所有LED
    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

/**
 * @brief   根据温湿度状态设置AHT20指示灯
 * @param   level: 0-正常 1-警告 2-异常
 */
void LED_AHT20_SetLevel(uint8_t level)
{
    // 先全部关闭
    GPIO_SetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
    
    switch(level) {
        case 0:  // 正常
            GPIO_ResetBits(GPIOB, GPIO_Pin_12);  // 点亮绿灯
            break;
        case 1:  // 警告
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);  // 点亮黄灯
            break;
        case 2:  // 异常
            GPIO_ResetBits(GPIOB, GPIO_Pin_14);  // 点亮红灯
            break;
    }
}

/**
 * @brief   根据气体状态设置MQ2指示灯
 * @param   level: 0-正常 1-警告 2-报警
 */
void LED_MQ2_SetLevel(uint8_t level)
{
    if (level == 0) {  // 正常
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);  // 点亮绿灯
    } else {
        GPIO_SetBits(GPIOB, GPIO_Pin_15);    // 熄灭绿灯
    }
}
