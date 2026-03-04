#include "I2C_Hardware.h"

/**
 * @brief  硬件I2C初始化。
 * @note   `SCL` - `PB6`
 * @note   `SDA` - `PB7`
 * @param  [in] speed 指定I2C时钟速度（Hz）, 最大400000Hz.
 * @retval 无
 */
void I2C_HW_Init(uint32_t speed)
{
    // 使能时钟 - I2C1 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);  // 改为 I2C1

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitTypeDef I2C_InitStructure;
    I2C_InitStructure.I2C_ClockSpeed          = speed;
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_OwnAddress1         = 0x00;
    I2C_Init(I2C1, &I2C_InitStructure);  // 改为 I2C1

    I2C_Cmd(I2C1, ENABLE);  // 改为 I2C1
}

// 下面所有 I2C2 都要改为 I2C1
void I2C_HW_WriteData(uint8_t addr, const uint8_t *data, uint16_t len)
{
    uint16_t timeout = 0;
    
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        timeout++;
        if(timeout > 10000) return;
    }

    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = 0;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS) {
        timeout++;
        if(timeout > 10000) {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return;
        }
    }

    I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Transmitter);
    timeout = 0;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS) {
        timeout++;
        if(timeout > 10000) {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return;
        }
    }

    for (uint16_t i = 0; i < len; i++)
    {
        I2C_SendData(I2C1, data[i]);
        timeout = 0;
        while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS) {
            timeout++;
            if(timeout > 10000) {
                I2C_GenerateSTOP(I2C1, ENABLE);
                return;
            }
        }
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C_HW_ReadData(uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t timeout = 0;
    
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET) {
        timeout++;
        if(timeout > 10000) return;
    }

    I2C_GenerateSTART(I2C1, ENABLE);
    timeout = 0;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS) {
        timeout++;
        if(timeout > 10000) {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return;
        }
    }

    I2C_Send7bitAddress(I2C1, addr, I2C_Direction_Receiver);
    timeout = 0;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS) {
        timeout++;
        if(timeout > 10000) {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return;
        }
    }

    for (uint16_t i = 0; i < len; i++)
    {
        if (i == len - 1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
        }

        timeout = 0;
        while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS) {
            timeout++;
            if(timeout > 10000) {
                I2C_GenerateSTOP(I2C1, ENABLE);
                I2C_AcknowledgeConfig(I2C1, ENABLE);
                return;
            }
        }
        buf[i] = I2C_ReceiveData(I2C1);
    }

    I2C_GenerateSTOP(I2C1, ENABLE);
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}