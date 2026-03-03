#ifndef __ACTION_H
#define __ACTION_H

#include "stm32f10x.h"

/* ========== 光照查看功能 ========== */
void Light_View(void);

/* ========== 温湿度查看功能 ========== */
void TempHum_View(void);

/* ========== 气体查看功能 ========== */
void Gas_View(void);

/* ========== 风扇控制功能 ========== */
void Fan_Manual(void);
void Fan_ON(void);
void Fan_OFF(void);
void Fan_Auto(void);

/* ========== 阈值查看功能 ========== */
void Gas_Threshold_View(void);
void Temp_Threshold_View(void);

/* ========== 系统状态查看功能 ========== */
void System_Status(void);
void Uptime_View(void);
void Alarms_View(void);
void Bluetooth_Status(void);

/* ========== 返回主菜单功能 ========== */
void Return_To_Main(void);

/* ========== 系统更新函数 ========== */
void Update_System_Uptime(void);

#endif /* __ACTION_H */
