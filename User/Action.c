#include "Action.h"
#include "stdint.h"
#include "OLED.h"
#include "LED.h"
#include "menu.h"
#include "Buzzer.h"
#include "Key.h"
#include "JDY-31.h"
#include "Light.h"        // 光照传感器
#include "AHT20.h"        // 温湿度传感器
#include "mq2.h"          // 气体传感器
#include "L9110.h"        // 风扇控制
#include <stdio.h>

// 静态变量存储数据
static float current_temp = 0;
static float current_humi = 0;
static uint16_t current_gas = 0;
static uint8_t current_light = 0;

// 系统状态变量
extern uint32_t system_uptime;
extern uint32_t alarm_count;
extern uint8_t bluetooth_connected;

// 阈值变量（仅用于显示）
// static uint16_t gas_normal_threshold = 500;   // 气体正常阈值
static uint16_t gas_warning_threshold = 800;  // 气体警告阈值
// static uint16_t gas_alarm_threshold = 1000;   // 气体报警阈值
// static uint16_t temp_normal_low = 200;        // 温度正常下限（20.0℃）
// static uint16_t temp_normal_high = 300;       // 温度正常上限（30.0℃）
// static uint16_t temp_cold_threshold = 150;    // 过冷阈值（15.0℃）
// static uint16_t temp_hot_threshold = 350;     // 过热阈值（35.0℃）

// 风扇控制变量
uint8_t fan_auto_mode = 1;  // 0:手动, 1:自动

// 外部函数声明
extern uint32_t Get_Tick(void);

/* ========== 光照查看功能 ========== */

/**
 * @brief   光照查看功能
 */
void Light_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t send_count = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Light ===");
    OLED_ShowString(2, 1, "Status:");
    OLED_ShowString(3, 1, "Value:");
    OLED_ShowString(4, 1, "Key4: Back");
    
    // 发送开始消息
    Send_Message_To_Phone("[LIGHT] Enter light view mode");
    
    while(!exit_flag) {
        // 读取光照状态（返回1:暗, 2:亮）
        current_light = LightSensor_Read();
        
        // 更新显示
        if (current_light == 2) {  // 亮
            OLED_ShowString(2, 8, "BRIGHT");
            OLED_ShowString(3, 7, "2 (Bright)");
            LEDG_ON();
            LEDR_OFF();
        } else if (current_light == 1) {  // 暗
            OLED_ShowString(2, 8, "DARK  ");
            OLED_ShowString(3, 7, "1 (Dark) ");
            LEDR_ON();
            LEDG_OFF();
        } else {
            OLED_ShowString(2, 8, "ERROR ");
            OLED_ShowString(3, 7, "Error   ");
        }
        
        // 每10次发送一次数据到手机
        send_count++;
        if (send_count >= 10) {
            char bt_message[50];
            if (current_light == 2) {
                sprintf(bt_message, "[LIGHT] Status: BRIGHT(2)");
            } else if (current_light == 1) {
                sprintf(bt_message, "[LIGHT] Status: DARK(1)");
            } else {
                sprintf(bt_message, "[LIGHT] Read Error");
            }
            Send_Message_To_Phone(bt_message);
            send_count = 0;
        }
        
        // 检查退出按键 - 每循环都检测
        uint8_t key_num = Key_GetNum();
        if (key_num == 4) {  // 按键4返回
            exit_flag = 1;
            Send_Message_To_Phone("[LIGHT] Exit light view mode");
        }
        
        Delay_ms(200);
    }
    
    // 退出前显示提示
    OLED_Clear();
    OLED_ShowString(3, 1, "Exiting...");
    Delay_ms(200);
    
    LEDR_OFF();
    LEDG_OFF();
    Menu_Display();
}

/* ========== 温湿度查看功能 ========== */

/**
 * @brief   温湿度查看功能
 */
void TempHum_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t send_count = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Temp&Hum ===");
    OLED_ShowString(2, 1, "Temp: --.-C");
    OLED_ShowString(3, 1, "Humi: --.-%");
    OLED_ShowString(4, 1, "Key4: Back");
    
    // 发送开始消息
    Send_Message_To_Phone("[T&H] Enter temperature & humidity view mode");
    
    while(!exit_flag) {
        // 读取温湿度数据
        uint8_t read_status = AHT20_ReadData(&current_temp, &current_humi);
        
        if (read_status == 0) {  // 读取成功
            // 显示温度（保留一位小数）
            uint16_t temp_int = (uint16_t)current_temp;
            uint16_t temp_dec = (uint16_t)(current_temp * 10) % 10;
            
            OLED_ShowString(2, 6, "   ");
            OLED_ShowNum(2, 6, temp_int, 2);
            OLED_ShowChar(2, 8, '.');
            OLED_ShowNum(2, 9, temp_dec, 1);
            OLED_ShowChar(2, 10, 'C');
            
            // 显示湿度（保留一位小数）
            uint16_t humi_int = (uint16_t)current_humi;
            uint16_t humi_dec = (uint16_t)(current_humi * 10) % 10;
            
            OLED_ShowString(3, 6, "   ");
            OLED_ShowNum(3, 6, humi_int, 2);
            OLED_ShowChar(3, 8, '.');
            OLED_ShowNum(3, 9, humi_dec, 1);
            OLED_ShowChar(3, 10, '%');
            
            // 根据温湿度指示LED
            // if (current_temp > 20 && current_temp < 30 && 
            //     current_humi > 40 && current_humi < 70) {
            //     LEDG_ON();
            //     LEDR_OFF();
            // } else {
            //     LEDR_ON();
            //     LEDG_OFF();
            // }
            
            // 每5次发送一次数据
            send_count++;
            if (send_count >= 5) {
                char bt_message[60];
                sprintf(bt_message, "[T&H] Temp: %.1fC, Humi: %.1f%%", 
                        current_temp, current_humi);
                Send_Message_To_Phone(bt_message);
                send_count = 0;
            }
        } else {
            // 读取失败
            OLED_ShowString(2, 6, "Error");
            OLED_ShowString(3, 6, "Error");
        }
        
        // 检查退出按键 - 每循环都检测
        uint8_t key_num = Key_GetNum();
        if (key_num == 4) {  // 按键4返回
            exit_flag = 1;
            Send_Message_To_Phone("[T&H] Exit temperature & humidity view mode");
        }
        
        Delay_ms(500);
    }
    
    // 退出前显示提示
    OLED_Clear();
    OLED_ShowString(3, 1, "Exiting...");
    Delay_ms(200);
    
    LEDR_OFF();
    LEDG_OFF();
    Menu_Display();
}

/* ========== 气体查看功能 ========== */

/**
 * @brief   气体查看功能
 */
void Gas_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t send_count = 0;
    SmokeLevel_TypeDef gas_level;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Gas ===");
    OLED_ShowString(2, 1, "Value: ----");
    OLED_ShowString(3, 1, "Level:");
    OLED_ShowString(4, 1, "Key3:Thresh Key4:Back");
    
    // 发送开始消息
    Send_Message_To_Phone("[GAS] Enter gas view mode");
    
    while(!exit_flag) {
        // 读取气体数据
        current_gas = MQ2_ReadAverage(3);
        gas_level = MQ2_GetLevel();
        
        // 显示气体值
        OLED_ShowString(2, 7, "    ");
        OLED_ShowNum(2, 7, current_gas, 4);
        
        // 显示气体等级
        switch(gas_level) {
            case SMOKE_LEVEL_NORMAL:
                OLED_ShowString(3, 7, "Normal ");
                break;
            case SMOKE_LEVEL_WARNING:
                OLED_ShowString(3, 7, "Warning");
                break;
            case SMOKE_LEVEL_ALARM:
                OLED_ShowString(3, 7, "ALARM! ");
                BUZZER_ON();
                Delay_ms(50);
                BUZZER_OFF();
                alarm_count++;  // 报警次数增加
                break;
            case SMOKE_LEVEL_ERROR:
                OLED_ShowString(3, 7, "Error  ");
                break;
        }
        
        // 每3次发送一次数据
        send_count++;
        if (send_count >= 3) {
            char bt_message[60];
            char* level_str;
            
            switch(gas_level) {
                case SMOKE_LEVEL_NORMAL: level_str = "Normal"; break;
                case SMOKE_LEVEL_WARNING: level_str = "Warning"; break;
                case SMOKE_LEVEL_ALARM: level_str = "ALARM"; break;
                default: level_str = "Error"; break;
            }
            
            sprintf(bt_message, "[GAS] Value: %d, Level: %s", 
                    current_gas, level_str);
            Send_Message_To_Phone(bt_message);
            send_count = 0;
        }
        
        // 检查按键
        uint8_t key_num = Key_GetNum();
        if (key_num == 3) {  // 按键3进入阈值查看
            Gas_Threshold_View();
            // 返回后重新显示
            OLED_Clear();
            OLED_ShowString(1, 1, "=== Gas ===");
            OLED_ShowString(2, 1, "Value: ----");
            OLED_ShowString(3, 1, "Level:");
            OLED_ShowString(4, 1, "Key3:Thresh Key4:Back");
        }
        else if (key_num == 4) {  // 按键4返回
            exit_flag = 1;
            Send_Message_To_Phone("[GAS] Exit gas view mode");
        }
        
        Delay_ms(300);
    }
    
    // 退出前显示提示
    OLED_Clear();
    OLED_ShowString(3, 1, "Exiting...");
    Delay_ms(200);
    
    Menu_Display();
}

/* ========== 风扇控制功能 ========== */

/**
 * @brief   风扇控制主功能
 */
void Fan_Manual(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Fan Control ===");
    OLED_ShowString(2, 1, "Mode: Manual");
    OLED_ShowString(3, 1, "Fan: ----");
    OLED_ShowString(4, 1, "-------------------");
    
    // 发送开始消息
    Send_Message_To_Phone("[FAN] Enter fan control mode");
    
    while(!exit_flag) {
        FanState_TypeDef fan_state = Fan_GetState();
        
        // 更新模式显示
        if (fan_auto_mode) {
            OLED_ShowString(2, 6, "Auto ");
        } else {
            OLED_ShowString(2, 6, "Manual");
        }
        
        // 更新风扇状态显示
        switch(fan_state) {
            case FAN_STOP:
                OLED_ShowString(3, 5, "Stop ");
                break;
            case FAN_FORWARD:
                OLED_ShowString(3, 5, "ON   ");
                break;
            case FAN_REVERSE:
                OLED_ShowString(3, 5, "REV  ");
                break;
            case FAN_BRAKE:
                OLED_ShowString(3, 5, "Brake");
                break;
        }
        
        // 按键处理
        uint8_t key_num = Key_GetNum();
        if (key_num != last_key) {
            if (key_num != 0) {
                switch(key_num) {
                    case 1: Fan_ON(); break;
                    case 2: Fan_OFF(); break;
                    case 3: Fan_Auto(); break;
                    case 4: exit_flag = 1; break;
                }
            }
            last_key = key_num;
        }
        
        // 自动模式逻辑
        if (fan_auto_mode) {
            current_gas = MQ2_ReadRaw();
            if (current_gas > gas_warning_threshold) {
                Fan_Forward();
            } else {
                Fan_Stop();
            }
        }
        
        Delay_ms(100);
    }
    
    Send_Message_To_Phone("[FAN] Exit fan control mode");
    Fan_Stop();
    Menu_Display();
}

/**
 * @brief   打开风扇
 */
void Fan_ON(void)
{
    Fan_Forward();
    fan_auto_mode = 0;
    Send_Message_To_Phone("[FAN] Manual: Fan ON");
    Delay_ms(50);
}

/**
 * @brief   关闭风扇
 */
void Fan_OFF(void)
{
    Fan_Stop();
    fan_auto_mode = 0;
    Send_Message_To_Phone("[FAN] Manual: Fan OFF");
    Delay_ms(50);
}

/**
 * @brief   自动模式
 */
void Fan_Auto(void)
{
    fan_auto_mode = 1;
    Send_Message_To_Phone("[FAN] Auto mode enabled");
    Delay_ms(50);
}

/* ========== 阈值查看功能 ========== */

/**
 * @brief   气体阈值查看
 */
void Gas_Threshold_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Gas Thresholds");
    OLED_ShowString(2, 1, "Normal < 500");
    OLED_ShowString(3, 1, "Warn 500-800");
    OLED_ShowString(4, 1, "Alarm > 800");
    
    Send_Message_To_Phone("[THRESH] View gas thresholds");
    
    while(!exit_flag) {
        uint8_t key_num = Key_GetNum();
        
        if (key_num != last_key) {
            if (key_num == 4) {  // 按键4返回
                exit_flag = 1;
            }
            last_key = key_num;
        }
        
        Delay_ms(100);
    }
}

/**
 * @brief   温度阈值查看
 */
void Temp_Threshold_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Temp Thresholds");
    OLED_ShowString(2, 1, "Comfort: 20-30C");
    OLED_ShowString(3, 1, "Cold: <15C");
    OLED_ShowString(4, 1, "Hot: >35C");
    
    Send_Message_To_Phone("[THRESH] View temperature thresholds");
    
    while(!exit_flag) {
        uint8_t key_num = Key_GetNum();
        
        if (key_num != last_key) {
            if (key_num == 4) {  // 按键4返回
                exit_flag = 1;
            }
            last_key = key_num;
        }
        
        Delay_ms(100);
    }
}

/* ========== 系统状态查看功能 ========== */

/**
 * @brief   系统状态主菜单
 */
void System_Status(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== System Status ===");
    OLED_ShowString(2, 1, "1.Uptime");
    OLED_ShowString(3, 1, "2.Alarms");
    OLED_ShowString(4, 1, "3.Bluetooth 4.Back");
    
    while(!exit_flag) {
        uint8_t key_num = Key_GetNum();
        
        if (key_num != last_key) {
            if (key_num == 1) {
                Uptime_View();
                // 返回后重新显示主菜单
                OLED_Clear();
                OLED_ShowString(1, 1, "=== System Status ===");
                OLED_ShowString(2, 1, "1.Uptime");
                OLED_ShowString(3, 1, "2.Alarms");
                OLED_ShowString(4, 1, "3.Bluetooth 4.Back");
            }
            else if (key_num == 2) {
                Alarms_View();
                OLED_Clear();
                OLED_ShowString(1, 1, "=== System Status ===");
                OLED_ShowString(2, 1, "1.Uptime");
                OLED_ShowString(3, 1, "2.Alarms");
                OLED_ShowString(4, 1, "3.Bluetooth 4.Back");
            }
            else if (key_num == 3) {
                Bluetooth_Status();
                OLED_Clear();
                OLED_ShowString(1, 1, "=== System Status ===");
                OLED_ShowString(2, 1, "1.Uptime");
                OLED_ShowString(3, 1, "2.Alarms");
                OLED_ShowString(4, 1, "3.Bluetooth 4.Back");
            }
            else if (key_num == 4) {
                exit_flag = 1;
            }
            last_key = key_num;
        }
        
        Delay_ms(100);
    }
    
    Menu_Display();
}

/**
 * @brief   运行时间查看
 */
void Uptime_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Uptime ===");
    
    uint32_t days = system_uptime / 86400;
    uint32_t hours = (system_uptime % 86400) / 3600;
    uint32_t minutes = (system_uptime % 3600) / 60;
    uint32_t seconds = system_uptime % 60;
    
    char line[17];
    sprintf(line, "%lud %luh %lum %lus", days, hours, minutes, seconds);
    OLED_ShowString(2, 1, line);
    OLED_ShowString(4, 1, "Key4: Back");
    
    Send_Message_To_Phone("[STATUS] View uptime");
    
    while(!exit_flag) {
        uint8_t key_num = Key_GetNum();
        if (key_num != last_key) {
            if (key_num == 4) {
                exit_flag = 1;
            }
            last_key = key_num;
        }
        Delay_ms(100);
    }
}

/**
 * @brief   报警次数查看
 */
void Alarms_View(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Alarms ===");
    
    char line[17];
    sprintf(line, "Total Alarms: %lu", alarm_count);
    OLED_ShowString(2, 1, line);
    OLED_ShowString(4, 1, "Key4: Back");
    
    char msg[50];
    sprintf(msg, "[STATUS] Total alarms: %lu", alarm_count);
    Send_Message_To_Phone(msg);
    
    while(!exit_flag) {
        uint8_t key_num = Key_GetNum();
        if (key_num != last_key) {
            if (key_num == 4) {
                exit_flag = 1;
            }
            last_key = key_num;
        }
        Delay_ms(100);
    }
}

/**
 * @brief   蓝牙状态查看
 */
void Bluetooth_Status(void)
{
    uint8_t exit_flag = 0;
    uint8_t last_key = 0;
    
    OLED_Clear();
    OLED_ShowString(1, 1, "=== Bluetooth ===");
    
    if (bluetooth_connected) {
        OLED_ShowString(2, 1, "Status: Connected");
        OLED_ShowString(3, 1, "Device: JDY-31");
        LEDG_ON();
        LEDR_OFF();
    } else {
        OLED_ShowString(2, 1, "Status: Disconnected");
        OLED_ShowString(3, 1, "Check Connection");
        LEDR_ON();
        LEDG_OFF();
    }
    
    OLED_ShowString(4, 1, "Key4: Back");
    
    Send_Message_To_Phone("[STATUS] Bluetooth status check");
    
    while(!exit_flag) {
        uint8_t key_num = Key_GetNum();
        if (key_num != last_key) {
            if (key_num == 4) {
                exit_flag = 1;
            }
            last_key = key_num;
        }
        Delay_ms(100);
    }
    
    LEDR_OFF();
    LEDG_OFF();
}

/* ========== 返回主菜单功能 ========== */

/**
 * @brief   直接返回主菜单
 */
void Return_To_Main(void)
{
    // 关闭所有LED
    LEDR_OFF();
    LEDG_OFF();
    LEDB_OFF();
    
    // 关闭蜂鸣器
    BUZZER_OFF();
    
    // 发送蓝牙消息
    Send_Message_To_Phone("[SYSTEM] Return to main menu");
    
    // 清屏并显示返回提示
    OLED_Clear();
    OLED_ShowString(2, 1, "Returning...");
    Delay_ms(300);
    
    // 调用菜单显示函数返回主菜单
    Menu_Display();
}

/* ========== 运行时间更新函数 ========== */

/**
 * @brief   更新运行时间（需要在主循环中调用）
 */
void Update_System_Uptime(void)
{
    static uint32_t last_second_tick = 0;
    uint32_t current_tick = Get_Tick();
    
    // 每100个tick（假设1 tick = 10ms）为1秒
    if (current_tick - last_second_tick >= 100) {
        system_uptime++;
        last_second_tick = current_tick;
    }
}
