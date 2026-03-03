// AHT20.c - 修改为软件I2C版本
#include "AHT20.h"
#include "Delay.h"
#include "stdint.h"
#include <cstddef>

/* 引脚定义 - 使用PB6/PB7软件I2C */
#define AHT20_SCL_PIN       GPIO_Pin_6
#define AHT20_SDA_PIN       GPIO_Pin_7
#define AHT20_GPIO_PORT     GPIOB
#define AHT20_GPIO_CLK      RCC_APB2Periph_GPIOB

/* I2C操作宏 */
#define SCL_HIGH()          GPIO_SetBits(AHT20_GPIO_PORT, AHT20_SCL_PIN)
#define SCL_LOW()           GPIO_ResetBits(AHT20_GPIO_PORT, AHT20_SCL_PIN)
#define SDA_HIGH()          GPIO_SetBits(AHT20_GPIO_PORT, AHT20_SDA_PIN)
#define SDA_LOW()           GPIO_ResetBits(AHT20_GPIO_PORT, AHT20_SDA_PIN)
#define SDA_READ()          GPIO_ReadInputDataBit(AHT20_GPIO_PORT, AHT20_SDA_PIN)

/* 内部函数声明 */
static void AHT20_GPIO_Init(void);
static void AHT20_I2C_Start(void);
static void AHT20_I2C_Stop(void);
static void AHT20_I2C_SendByte(uint8_t data);
static uint8_t AHT20_I2C_ReceiveByte(void);
static uint8_t AHT20_I2C_WaitAck(void);
static void AHT20_I2C_Ack(void);
static void AHT20_I2C_NAck(void);
static uint8_t AHT20_WriteCmd(uint8_t cmd, uint8_t *data, uint8_t len);
static uint8_t AHT20_ReadData(uint8_t *data, uint8_t len);
static uint8_t AHT20_WaitForIdle(void);

/**
 * @brief   软件延时（I2C时序用）
 */
static void I2C_Delay(void)
{
    uint8_t i = 5;
    while(i--);
}

/**
 * @brief   初始化GPIO
 */
static void AHT20_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(AHT20_GPIO_CLK, ENABLE);
    
    // 配置PB6/PB7为推挽输出
    GPIO_InitStructure.GPIO_Pin = AHT20_SCL_PIN | AHT20_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AHT20_GPIO_PORT, &GPIO_InitStructure);
    
    // 初始状态：SCL和SDA高电平
    SCL_HIGH();
    SDA_HIGH();
}

/**
 * @brief   I2C起始信号
 */
static void AHT20_I2C_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    SDA_LOW();
    I2C_Delay();
    SCL_LOW();
}

/**
 * @brief   I2C停止信号
 */
static void AHT20_I2C_Stop(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_Delay();
    SDA_HIGH();
    I2C_Delay();
}

/**
 * @brief   I2C发送一个字节
 * @param   data: 要发送的数据
 */
static void AHT20_I2C_SendByte(uint8_t data)
{
    uint8_t i;
    
    for (i = 0; i < 8; i++) {
        if (data & 0x80) {
            SDA_HIGH();
        } else {
            SDA_LOW();
        }
        data <<= 1;
        I2C_Delay();
        SCL_HIGH();
        I2C_Delay();
        SCL_LOW();
        I2C_Delay();
    }
}

/**
 * @brief   I2C接收一个字节
 * @retval  接收到的数据
 */
static uint8_t AHT20_I2C_ReceiveByte(void)
{
    uint8_t i, data = 0;
    
    // 设置为输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = AHT20_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(AHT20_GPIO_PORT, &GPIO_InitStructure);
    
    for (i = 0; i < 8; i++) {
        data <<= 1;
        SCL_HIGH();
        I2C_Delay();
        if (SDA_READ()) {
            data |= 0x01;
        }
        SCL_LOW();
        I2C_Delay();
    }
    
    // 恢复为输出模式
    GPIO_InitStructure.GPIO_Pin = AHT20_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AHT20_GPIO_PORT, &GPIO_InitStructure);
    
    return data;
}

/**
 * @brief   等待应答信号
 * @retval  0:收到应答 1:未收到应答
 */
static uint8_t AHT20_I2C_WaitAck(void)
{
    uint8_t ack = 0;
    uint16_t timeout = 0;
    
    // 设置为输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = AHT20_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(AHT20_GPIO_PORT, &GPIO_InitStructure);
    
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    
    while (SDA_READ() && timeout < 100) {
        timeout++;
        I2C_Delay();
    }
    
    if (timeout >= 100) {
        ack = 1;  // 超时，无应答
    }
    
    SCL_LOW();
    I2C_Delay();
    
    // 恢复为输出模式
    GPIO_InitStructure.GPIO_Pin = AHT20_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AHT20_GPIO_PORT, &GPIO_InitStructure);
    
    return ack;
}

/**
 * @brief   发送应答信号
 */
static void AHT20_I2C_Ack(void)
{
    SDA_LOW();
    SCL_HIGH();
    I2C_Delay();
    SCL_LOW();
    SDA_HIGH();
}

/**
 * @brief   发送非应答信号
 */
static void AHT20_I2C_NAck(void)
{
    SDA_HIGH();
    SCL_HIGH();
    I2C_Delay();
    SCL_LOW();
}

/**
 * @brief   AHT20初始化
 */
uint8_t AHT20_Init(void)
{
    uint8_t status;
    uint8_t init_cmd[3] = {0x08, 0x00, 0x00};
    
    // 1. 初始化GPIO
    AHT20_GPIO_Init();
    Delay_ms(50);
    
    // 2. 软复位
    AHT20_SoftReset();
    Delay_ms(20);
    
    // 3. 发送初始化命令
    if (AHT20_WriteCmd(AHT20_CMD_INIT, init_cmd, 3) != 0) {
        return 1;
    }
    Delay_ms(10);
    
    // 4. 检查校准状态
    status = AHT20_ReadStatus();
    if (status != 0xFF && (status & AHT20_STATUS_CAL)) {
        return 0;  // 校准成功
    }
    
    return 1;  // 校准失败
}

/**
 * @brief   读取温湿度数据
 */
uint8_t AHT20_ReadTempHum(float *temperature, float *humidity)
{
    uint8_t buf[6] = {0};
    uint32_t raw_humi = 0, raw_temp = 0;
    
    if (temperature == NULL || humidity == NULL) {
        return 1;
    }
    
    // 1. 触发测量
    if (AHT20_WriteCmd(AHT20_CMD_TRIGGER, NULL, 0) != 0) {
        return 1;
    }
    
    // 2. 等待测量完成
    Delay_ms(80);
    
    // 3. 检查传感器状态
    if (AHT20_WaitForIdle() != 0) {
        return 1;
    }
    
    // 4. 读取6字节数据
    if (AHT20_ReadData(buf, 6) != 0) {
        return 1;
    }
    
    // 5. 校验状态字节
    if ((buf[0] & 0x80) != 0) {
        return 1;
    }
    
    // 6. 解析原始数据
    raw_humi = ((uint32_t)buf[1] << 12) | ((uint32_t)buf[2] << 4) | ((uint32_t)buf[3] >> 4);
    raw_temp = (((uint32_t)buf[3] & 0x0F) << 16) | ((uint32_t)buf[4] << 8) | (uint32_t)buf[5];
    
    // 7. 转换为实际物理量
    *humidity = (float)raw_humi * 100.0f / 1048576.0f;
    *temperature = (float)raw_temp * 200.0f / 1048576.0f - 50.0f;
    
    return 0;
}

/**
 * @brief   软复位传感器
 */
uint8_t AHT20_SoftReset(void)
{
    return AHT20_WriteCmd(AHT20_CMD_SOFTRESET, NULL, 0);
}

/**
 * @brief   读取状态寄存器
 */
uint8_t AHT20_ReadStatus(void)
{
    uint8_t status = 0;
    
    if (AHT20_WriteCmd(AHT20_CMD_STATUS, NULL, 0) != 0) {
        return 0xFF;
    }
    
    if (AHT20_ReadData(&status, 1) != 0) {
        return 0xFF;
    }
    
    return status;
}

/**
 * @brief   写命令
 */
static uint8_t AHT20_WriteCmd(uint8_t cmd, uint8_t *data, uint8_t len)
{
    uint8_t i;
    
    AHT20_I2C_Start();
    
    // 发送器件地址 + 写
    AHT20_I2C_SendByte(AHT20_ADDR);
    if (AHT20_I2C_WaitAck()) {
        AHT20_I2C_Stop();
        return 1;
    }
    
    // 发送命令字
    AHT20_I2C_SendByte(cmd);
    if (AHT20_I2C_WaitAck()) {
        AHT20_I2C_Stop();
        return 1;
    }
    
    // 发送附加数据
    for (i = 0; i < len; i++) {
        AHT20_I2C_SendByte(data[i]);
        if (AHT20_I2C_WaitAck()) {
            AHT20_I2C_Stop();
            return 1;
        }
    }
    
    AHT20_I2C_Stop();
    
    return 0;
}

/**
 * @brief   读取数据
 */
static uint8_t AHT20_ReadData(uint8_t *data, uint8_t len)
{
    uint8_t i;
    
    if (data == NULL || len == 0) {
        return 1;
    }
    
    AHT20_I2C_Start();
    
    // 发送器件地址 + 读
    AHT20_I2C_SendByte(AHT20_ADDR | 0x01);
    if (AHT20_I2C_WaitAck()) {
        AHT20_I2C_Stop();
        return 1;
    }
    
    // 读取数据
    for (i = 0; i < len; i++) {
        data[i] = AHT20_I2C_ReceiveByte();
        if (i == len - 1) {
            AHT20_I2C_NAck();  // 最后一个字节发非应答
        } else {
            AHT20_I2C_Ack();    // 其他字节发应答
        }
    }
    
    AHT20_I2C_Stop();
    
    return 0;
}

/**
 * @brief   等待传感器空闲
 */
static uint8_t AHT20_WaitForIdle(void)
{
    uint16_t timeout = 0;
    uint8_t status;
    
    while (timeout < AHT20_TIMEOUT) {
        status = AHT20_ReadStatus();
        if (status == 0xFF) {
            return 1;
        }
        if (!(status & AHT20_STATUS_BUSY)) {
            return 0;
        }
        Delay_ms(1);
        timeout++;
    }
    
    return 1;
}