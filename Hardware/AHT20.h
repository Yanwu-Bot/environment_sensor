#ifndef __AHT20_H
#define __AHT20_H

#include "I2C_Hardware.h"

typedef enum
{
    AHT20_OK    = 0,
    AHT20_ERROR = 1,
} Aht20_Status_en_t;

void AHT20_Init(void);
Aht20_Status_en_t AHT20_ReadData(float *temp, float *humi);

#endif /* __AHT20_H */
