// test_aht20_raw.c
#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "AHT20.h"
#include "stdio.h"

uint32_t system_uptime = 0;
uint32_t alarm_count = 0;
uint8_t bluetooth_connected = 1;

uint32_t Get_Tick(void)
{
    static uint32_t tick = 0;
    return tick++;
}

// 声明I2C函数
extern void I2C_HW_Init(uint32_t speed);
extern void I2C_HW_WriteData(uint8_t addr, const uint8_t *data, uint16_t len);
extern void I2C_HW_ReadData(uint8_t addr, uint8_t *buf, uint16_t len);

#define AHT20_ADDRESS (0x70)

int main(void)
{
    OLED_Init();
    Delay_ms(200);
    
    OLED_Clear();
    OLED_ShowString(1, 1, "AHT20 Raw Data");
    
    // 初始化I2C
    I2C_HW_Init(100000);
    Delay_ms(100);
    
    // 发送初始化命令
    uint8_t init_cmd[3] = {0xBE, 0x08, 0x00};
    I2C_HW_WriteData(AHT20_ADDRESS, init_cmd, 3);
    Delay_ms(50);
    
    uint8_t data[6];
    char str[17];
    uint16_t count = 0;
    
    while(1)
    {
        count++;
        
        // 发送测量命令
        uint8_t measure_cmd[3] = {0xAC, 0x33, 0x00};
        I2C_HW_WriteData(AHT20_ADDRESS, measure_cmd, 3);
        Delay_ms(100);
        
        // 读取数据
        I2C_HW_ReadData(AHT20_ADDRESS, data, 6);
        
        // 第1行：显示计数和状态
        sprintf(str, "C:%d S:%02X", count, data[0]);
        OLED_ShowString(1, 1, str);
        
        // 第2行：显示前3个字节
        sprintf(str, "%02X %02X %02X", data[0], data[1], data[2]);
        OLED_ShowString(2, 1, str);
        
        // 第3行：显示后3个字节
        sprintf(str, "%02X %02X %02X", data[3], data[4], data[5]);
        OLED_ShowString(3, 1, str);
        
        // 第4行：显示解析结果
        if((data[0] & 0x80) == 0) {
            uint32_t humi_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
            uint32_t temp_raw = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];
            
            float humi = humi_raw * 100.0f / 1048576.0f;
            float temp = temp_raw * 200.0f / 1048576.0f - 50.0f;
            
            sprintf(str, "T:%.1f H:%.1f", temp, humi);
            OLED_ShowString(4, 1, str);
        } else {
            OLED_ShowString(4, 1, "BUSY");
        }
        
        Delay_ms(1000);
    }
}