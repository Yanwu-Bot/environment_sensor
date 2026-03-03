#ifndef __MQ2_H
#define __MQ2_H

#include "stm32f10x.h"

/* 引脚定义 - 使用PB0 (ADC1_IN8) */
#define MQ2_ADC_CHANNEL         ADC_Channel_8
#define MQ2_ADC_PORT            GPIOB
#define MQ2_ADC_PIN             GPIO_Pin_0
#define MQ2_ADC_CLK             RCC_APB2Periph_GPIOB
#define MQ2_ADC                  ADC1

/* 阈值定义 (根据实际测试调整) */
#define MQ2_THRESHOLD_NORMAL     500    // 正常阈值
#define MQ2_THRESHOLD_WARNING    800    // 警告阈值
#define MQ2_THRESHOLD_ALARM      1000   // 报警阈值

/* 烟雾等级枚举 */
typedef enum {
    SMOKE_LEVEL_NORMAL = 0,      // 正常
    SMOKE_LEVEL_WARNING,         // 警告
    SMOKE_LEVEL_ALARM,           // 报警
    SMOKE_LEVEL_ERROR            // 错误
} SmokeLevel_TypeDef;

/* 函数声明 */
void MQ2_Init(void);
uint16_t MQ2_ReadRaw(void);
uint16_t MQ2_ReadAverage(uint8_t samples);
float MQ2_ReadPercentage(void);
SmokeLevel_TypeDef MQ2_GetLevel(void);
uint8_t MQ2_IsNormal(void);
uint8_t MQ2_IsWarning(void);
uint8_t MQ2_IsAlarm(void);
void MQ2_SetThreshold(uint16_t normal, uint16_t warning, uint16_t alarm);
void MQ2_DisplayStatus(void);

#endif /* __MQ2_H */
