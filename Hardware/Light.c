#include "Light.h"
#include "Delay.h"

/**
 * @brief   光敏传感器初始化
 * @param   无
 * @retval  无
 * @note    配置PB1为输入模式
 *          暗→高电平，亮→低电平
 */
void LightSensor_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(LIGHT_SENSOR_CLK, ENABLE);
    
    // 配置为浮空输入（模块内部已有上拉/下拉电阻）
    GPIO_InitStructure.GPIO_Pin = LIGHT_SENSOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(LIGHT_SENSOR_PORT, &GPIO_InitStructure);
}

/**
 * @brief   读取光敏传感器状态
 * @param   无
 * @retval  1: 暗 (高电平)
 *          2: 亮 (低电平)
 * @note    暗→高电平返回1，亮→低电平返回2
 */
uint8_t LightSensor_Read(void)
{
    // 根据电平返回对应的值
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1) {
        return LIGHT_DARK;      // 高电平 → 暗 → 返回1
    } 
    if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
    {
        return LIGHT_BRIGHT;    // 低电平 → 亮 → 返回2
    }

    return 0;
}

/**
 * @brief   判断是否亮
 * @param   无
 * @retval  1: 亮 0: 不亮（暗）
 */
uint8_t LightSensor_IsBright(void)
{
    return (LightSensor_Read() == LIGHT_BRIGHT) ? 1 : 0;
}

/**
 * @brief   判断是否暗
 * @param   无
 * @retval  1: 暗 0: 不暗（亮）
 */
uint8_t LightSensor_IsDark(void)
{
    return (LightSensor_Read() == LIGHT_DARK) ? 1 : 0;
}

/**
 * @brief   显示当前状态（用于调试）
 * @param   无
 * @retval  无
 */

/**
 * @brief   带防抖动的读取
 * @param   delay_ms: 采样间隔(ms)
 * @retval  0或1
 */
uint8_t LightSensor_ReadDebounce(uint16_t delay_ms)
{
    uint8_t state1, state2;
    
    // 第一次读取
    state1 = LightSensor_Read();
    Delay_ms(delay_ms);
    
    // 第二次读取
    state2 = LightSensor_Read();
    
    // 如果两次读取一致，返回该状态；否则返回第一次的状态
    if (state1 == state2) {
        return state1;
    } else {
        return state1;
    }
}
