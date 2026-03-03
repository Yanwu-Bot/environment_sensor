#ifndef __LIGHT_H
#define __LIGHT_H

#include "stm32f10x.h"

/* 引脚定义 - 使用PB1 */
#define LIGHT_SENSOR_PIN        GPIO_Pin_1
#define LIGHT_SENSOR_PORT       GPIOB
#define LIGHT_SENSOR_CLK        RCC_APB2Periph_GPIOB

/* 亮度定义 */
#define LIGHT_DARK      1    // 暗（高电平）
#define LIGHT_BRIGHT    2    // 亮（低电平）

/* 函数声明 */
void LightSensor_Init(void);
uint8_t LightSensor_Read(void);           // 返回0或1
uint8_t LightSensor_IsBright(void);       // 返回1表示亮
uint8_t LightSensor_IsDark(void);          // 返回1表示暗


#endif /* __LIGHTSENSOR_H */
