#include "MQ2.h"
#include "Delay.h"  // 使用你的延时库

/* 静态变量存储阈值 */
static uint16_t mq2_threshold_normal = MQ2_THRESHOLD_NORMAL;
static uint16_t mq2_threshold_warning = MQ2_THRESHOLD_WARNING;
static uint16_t mq2_threshold_alarm = MQ2_THRESHOLD_ALARM;

/**
 * @brief   MQ-2传感器初始化
 * @param   无
 * @retval  无
 * @note    配置PB0为ADC输入
 */
void MQ2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(MQ2_ADC_CLK | RCC_APB2Periph_ADC1, ENABLE);
    
    // 2. 配置PB0为模拟输入
    GPIO_InitStructure.GPIO_Pin = MQ2_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(MQ2_ADC_PORT, &GPIO_InitStructure);
    
    // 3. ADC配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(MQ2_ADC, &ADC_InitStructure);
    
    // 4. 配置ADC通道（采样时间55.5周期）
    ADC_RegularChannelConfig(MQ2_ADC, MQ2_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    
    // 5. 使能ADC
    ADC_Cmd(MQ2_ADC, ENABLE);
    
    // 6. 校准ADC
    ADC_ResetCalibration(MQ2_ADC);
    while(ADC_GetResetCalibrationStatus(MQ2_ADC));
    ADC_StartCalibration(MQ2_ADC);
    while(ADC_GetCalibrationStatus(MQ2_ADC));
    
    // 7. 第一次读取预热
    MQ2_ReadRaw();
    Delay_ms(100);  // 使用你的延时函数
}

/**
 * @brief   读取ADC原始值
 * @param   无
 * @retval  0-4095 ADC值
 */
uint16_t MQ2_ReadRaw(void)
{
    // 启动ADC转换
    ADC_SoftwareStartConvCmd(MQ2_ADC, ENABLE);
    
    // 等待转换完成
    while(!ADC_GetFlagStatus(MQ2_ADC, ADC_FLAG_EOC));
    
    // 读取转换结果
    return ADC_GetConversionValue(MQ2_ADC);
}

/**
 * @brief   读取多次取平均（滤波）
 * @param   samples: 采样次数
 * @retval  平均ADC值
 */
uint16_t MQ2_ReadAverage(uint8_t samples)
{
    uint32_t sum = 0;
    uint8_t i;
    
    if (samples == 0) samples = 1;
    
    for (i = 0; i < samples; i++) {
        sum += MQ2_ReadRaw();
        if (i < samples - 1) {
            Delay_ms(10);  // 每次采样间隔10ms，使用你的延时函数
        }
    }
    
    return (uint16_t)(sum / samples);
}

/**
 * @brief   读取烟雾浓度百分比
 * @param   无
 * @retval  0.0 - 100.0 % (近似值)
 */
float MQ2_ReadPercentage(void)
{
    uint16_t raw = MQ2_ReadAverage(5);  // 5次采样取平均
    float percentage;
    
    // 简单的线性转换
    percentage = (float)raw * 100.0f / 4095.0f;
    
    // 限制范围
    if (percentage > 100.0f) percentage = 100.0f;
    if (percentage < 0.0f) percentage = 0.0f;
    
    return percentage;
}

/**
 * @brief   获取当前烟雾等级
 * @param   无
 * @retval  SmokeLevel_TypeDef
 */
SmokeLevel_TypeDef MQ2_GetLevel(void)
{
    uint16_t raw = MQ2_ReadAverage(3);  // 3次采样取平均
    
    if (raw < mq2_threshold_normal) {
        return SMOKE_LEVEL_NORMAL;
    } else if (raw < mq2_threshold_warning) {
        return SMOKE_LEVEL_WARNING;
    } else if (raw < mq2_threshold_alarm) {
        return SMOKE_LEVEL_ALARM;
    } else {
        return SMOKE_LEVEL_ERROR;
    }
}

/**
 * @brief   判断是否正常
 * @param   无
 * @retval  1: 正常 0: 不正常
 */
uint8_t MQ2_IsNormal(void)
{
    return (MQ2_GetLevel() == SMOKE_LEVEL_NORMAL) ? 1 : 0;
}

/**
 * @brief   判断是否警告
 * @param   无
 * @retval  1: 警告 0: 非警告
 */
uint8_t MQ2_IsWarning(void)
{
    return (MQ2_GetLevel() == SMOKE_LEVEL_WARNING) ? 1 : 0;
}

/**
 * @brief   判断是否报警
 * @param   无
 * @retval  1: 报警 0: 非报警
 */
uint8_t MQ2_IsAlarm(void)
{
    return (MQ2_GetLevel() == SMOKE_LEVEL_ALARM) ? 1 : 0;
}

/**
 * @brief   设置阈值
 * @param   normal: 正常阈值
 * @param   warning: 警告阈值  
 * @param   alarm: 报警阈值
 * @retval  无
 */
void MQ2_SetThreshold(uint16_t normal, uint16_t warning, uint16_t alarm)
{
    mq2_threshold_normal = normal;
    mq2_threshold_warning = warning;
    mq2_threshold_alarm = alarm;
}

/**
 * @brief   显示状态信息（调试用）
 * @param   无
 * @retval  无
 */
void MQ2_DisplayStatus(void)
{
    uint16_t raw = MQ2_ReadRaw();
    float percentage = MQ2_ReadPercentage();
    SmokeLevel_TypeDef level = MQ2_GetLevel();
    
    // 注意：如果要用printf，需要先实现串口打印
    // 这里假设你已经有了串口输出功能
}
