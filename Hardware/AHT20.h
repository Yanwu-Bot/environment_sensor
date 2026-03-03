#ifndef __AHT20_H
#define __AHT20_H

#include "stm32f10x.h"

/* 器件地址 */
#define AHT20_ADDR          0x70    // 7位地址，写操作时为0x70，读操作时为0x71

/* 命令字 */
#define AHT20_CMD_INIT       0xBE    // 初始化命令
#define AHT20_CMD_TRIGGER    0xAC    // 触发测量命令
#define AHT20_CMD_SOFTRESET  0xBA    // 软复位命令
#define AHT20_CMD_STATUS     0x71    // 读取状态命令

/* 状态寄存器位 */
#define AHT20_STATUS_BUSY    0x01    // Bit0: 1-忙碌 0-空闲
#define AHT20_STATUS_CAL     0x08    // Bit3: 1-已校准 0-未校准

/* 超时时间定义 */
#define AHT20_TIMEOUT        500     // 超时时间(ms)

/* 函数声明 */
uint8_t AHT20_Init(void);
uint8_t AHT20_ReadTempHum(float *temperature, float *humidity);
uint8_t AHT20_SoftReset(void);
uint8_t AHT20_ReadStatus(void);

#endif /* __AHT20_H */
