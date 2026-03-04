#ifndef __I2C_HARDWARE_H
#define __I2C_HARDWARE_H

#include "stm32f10x.h"

void I2C_HW_Init(uint32_t speed);
void I2C_HW_WriteData(uint8_t addr, const uint8_t *data, uint16_t len);
void I2C_HW_ReadData(uint8_t addr, uint8_t *buf, uint16_t len);

#endif /* __I2C_HARDWARE_H */
