#include "AHT20.h"
#include "Delay.h"
#include "I2C_Hardware.h"  // 添加头文件

#define AHT20_ADDRESS (0x70) /* (0x38 << 1) */

#define AHT20_CMD_INIT        (0xBE)
#define AHT20_CMD_INIT_PARAM1 (0x08)
#define AHT20_CMD_INIT_PARAM2 (0x00)

#define AHT20_CMD_MEASURE        (0xAC)
#define AHT20_CMD_MEASURE_PARAM1 (0x33)
#define AHT20_CMD_MEASURE_PARAM2 (0x00)

#define AHT20_CMD_SOFT_RESET (0xBA)

#define AHT20_STATUS_BUSY_MASK (0x80)

/**
 * @brief  初始化AHT20.
 */
void AHT20_Init(void)
{
    uint8_t cmd[3] = {AHT20_CMD_INIT, AHT20_CMD_INIT_PARAM1, AHT20_CMD_INIT_PARAM2};
    
    // 先初始化I2C
    I2C_HW_Init(100000);  // 100kHz
    Delay_ms(10);

    Delay_ms(40);
    I2C_HW_WriteData(AHT20_ADDRESS, cmd, 3);
    Delay_ms(20);
}

/**
 * @brief  读取AHT20温湿度数据.
 */
Aht20_Status_en_t AHT20_ReadData(float *temp, float *humi)
{
    uint8_t cmd[3]  = {AHT20_CMD_MEASURE, AHT20_CMD_MEASURE_PARAM1, AHT20_CMD_MEASURE_PARAM2};
    uint8_t data[6] = {0};
    uint32_t humi_raw, temp_raw;

    I2C_HW_WriteData(AHT20_ADDRESS, cmd, 3);

    Delay_ms(80);

    I2C_HW_ReadData(AHT20_ADDRESS, data, 6);

    /* 检查是否为忙状态 */
    if ((data[0] & AHT20_STATUS_BUSY_MASK) != 0)
    {
        return AHT20_ERROR;
    }

    /* 解析湿度 */
    humi_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    *humi    = humi_raw * 100.0f / (1 << 20);

    /* 解析温度 */
    temp_raw = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
    *temp    = temp_raw * 200.0f / (1 << 20) - 50;

    return AHT20_OK;
}