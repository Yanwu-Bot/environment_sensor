// main.c - 在主循环中添加LED自动控制
#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "menu.h"
#include "LED.h"
#include "Buzzer.h"
#include "JDY-31.h"
#include "Light.h"        // 光照传感器
#include "AHT20.h"        // 温湿度传感器
#include "mq2.h"          // 气体传感器
#include "L9110.h"        // 风扇控制
#include "Action.h"       // 功能函数

// 定义全局变量
uint32_t system_uptime = 0;  // 运行时间（秒）
uint32_t alarm_count = 0;     // 报警次数
uint8_t bluetooth_connected = 1;  // 蓝牙连接状态

// 阈值定义
#define GAS_NORMAL_THRESHOLD    500
#define GAS_WARNING_THRESHOLD   1800
#define GAS_ALARM_THRESHOLD     2500

#define TEMP_NORMAL_LOW         200  // 20.0℃
#define TEMP_NORMAL_HIGH        300  // 30.0℃
#define TEMP_COLD_THRESHOLD     150  // 15.0℃
#define TEMP_HOT_THRESHOLD      350  // 35.0℃

#define HUMI_NORMAL_LOW         40   // 40%
#define HUMI_NORMAL_HIGH        70   // 70%

// 传感器数据缓存
static float current_temp = 0;
static float current_humi = 0;
static uint16_t current_gas = 0;
static uint8_t current_light = 0;

// 状态标志
static uint8_t temp_hum_warning = 0;  // 温湿度警告状态
static uint8_t gas_alarm = 0;         // 气体报警状态


// 简单的Get_Tick函数
uint32_t Get_Tick(void)
{
    static uint32_t tick = 0;
    return tick++;
}

/**
 * @brief   更新温湿度状态和LED
 * @note    PB12: 警告/异常时亮红灯
 *          PB13: 正常时亮绿灯
 */
void Update_AHT20_LED(void)
{
    // 判断温湿度是否在正常范围
    if (current_temp >= 20 && current_temp <= 30 && 
        current_humi >= 40 && current_humi <= 70) {
        // 正常：PB13亮绿灯，PB12熄灭
        GPIO_SetBits(GPIOB, GPIO_Pin_12);  // 熄灭红灯
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); // 点亮绿灯
        temp_hum_warning = 0;
    } else {
        // 警告或异常：PB12亮红灯，PB13熄灭
        GPIO_ResetBits(GPIOB, GPIO_Pin_12); // 点亮红灯
        GPIO_SetBits(GPIOB, GPIO_Pin_13);   // 熄灭绿灯
        temp_hum_warning = 1;
    }
}

/**
 * @brief   更新气体状态和LED
 * @note    PB14: 严重阈值时亮红灯
 *          PB15: 正常时亮绿灯
 */
void Update_MQ2_LED(void)
{
    if (current_gas >= GAS_ALARM_THRESHOLD) {
        // 严重阈值：PB14亮红灯，PB15熄灭
        GPIO_ResetBits(GPIOB, GPIO_Pin_14); // 点亮红灯
        GPIO_SetBits(GPIOB, GPIO_Pin_15);   // 熄灭绿灯
        gas_alarm = 1;
    } else {
        // 正常：PB15亮绿灯，PB14熄灭
        GPIO_SetBits(GPIOB, GPIO_Pin_14);   // 熄灭红灯
        GPIO_ResetBits(GPIOB, GPIO_Pin_15); // 点亮绿灯
        gas_alarm = 0;
    }
}

/**
 * @brief   更新系统状态LED
 * @note    PA10: 全部正常时亮绿灯
 *          PA9:  有警告时亮蓝灯
 *          PA8:  有报警时亮红灯
 */
void Update_System_LED(void)
{
    int system_num;
    // 更新系统状态
    //安全
    if(temp_hum_warning && gas_alarm)
    {
        system_num = 0;
    }
    //警告
    else if((!temp_hum_warning && gas_alarm) || (!gas_alarm && temp_hum_warning))
    {
        system_num = 1;
    }
    //报警
    else
    {
        system_num = 2;
    }
    if (system_num == 2) {
        // 有报警：PA8亮红灯，其他熄灭
        GPIO_ResetBits(GPIOA, GPIO_Pin_8);   // 红灯亮
        GPIO_SetBits(GPIOA, GPIO_Pin_9);     // 蓝灯灭
        GPIO_SetBits(GPIOA, GPIO_Pin_10);    // 绿灯灭
    }
    else if (system_num == 1) {
        // 有警告：PA9亮蓝灯，其他熄灭
        GPIO_SetBits(GPIOA, GPIO_Pin_8);     // 红灯灭
        GPIO_ResetBits(GPIOA, GPIO_Pin_9);   // 蓝灯亮
        GPIO_SetBits(GPIOA, GPIO_Pin_10);    // 绿灯灭
    }
    else {
        // 全部正常：PA10亮绿灯，其他熄灭
        GPIO_SetBits(GPIOA, GPIO_Pin_8);     // 红灯灭
        GPIO_SetBits(GPIOA, GPIO_Pin_9);     // 蓝灯灭
        GPIO_ResetBits(GPIOA, GPIO_Pin_10);  // 绿灯亮
    }
}

/**
 * @brief   更新所有传感器数据
 */
void Update_Sensor_Data(void)
{
    // 读取光照
    current_light = LightSensor_Read();
    
    // 读取温湿度
    AHT20_ReadData(&current_temp, &current_humi);
    
    // 读取气体
    current_gas = MQ2_ReadAverage(3);
}

/**
 * @brief   主函数
 */
int main(void)
{
    // 1. 初始化OLED
    OLED_Init();
    Delay_ms(200);
    
    // 2. 显示启动画面
    OLED_Clear();
    OLED_ShowString(1, 1, "System Starting");
    OLED_ShowString(2, 1, "STM32F103C8T6");
    OLED_ShowString(3, 1, "Please wait...");
    OLED_ShowString(4, 1, "Ver 1.0");
    Delay_ms(1000);
    
    // 3. 初始化外设
    LED_Init();      // 初始化所有LED
    BUZZER_Init();
    Key_Init();
    UART3_init();
    
    // 4. 初始化传感器
    LightSensor_Init();
    AHT20_Init();
    MQ2_Init();
    Fan_Init();
    
    // 5. 初始化菜单
    Menu_Init();
    Menu_Display();
    
    // 6. 发送启动消息
    Send_Message_To_Phone("System Started");
    
    // 7. 主循环
    uint32_t last_sensor_read = 0;
    uint32_t last_uptime_update = 0;
    uint32_t alarm_counter = 0;
    
    while(1)
    {
        // 7.1 按键处理
        uint8_t Key_num = Key_GetNum();
        if (Key_num == 1) Menu_Up();
        else if (Key_num == 2) Menu_Down();
        else if (Key_num == 3) Menu_Enter();
        else if (Key_num == 4) Menu_Back();
        
        // 7.2 获取当前tick
        uint32_t current_tick = Get_Tick();
        
        // 7.3 每秒更新运行时间
        if (current_tick - last_uptime_update >= 100) {
            last_uptime_update = current_tick;
            system_uptime++;
        }
        
        // 7.4 每500ms读取一次传感器数据并更新LED
        if (current_tick - last_sensor_read >= 50) {  // 50 * 10ms = 500ms
            last_sensor_read = current_tick;
            
            // 读取所有传感器数据
            Update_Sensor_Data();
            
            // 更新所有LED状态
            Update_AHT20_LED();    // 更新温湿度LED (PB12/PB13)
            Update_MQ2_LED();       // 更新气体LED (PB14/PB15)
            Update_System_LED();    // 更新系统LED (PA8/PA9/PA10)
        }
        
        // 7.5 检查报警（每200次循环约2秒）
        if (current_tick - alarm_counter >= 200) {
            alarm_counter = current_tick;
            
            // 气体报警时蜂鸣器响
            if (current_gas >= GAS_ALARM_THRESHOLD) {
                BUZZER_ON();
                Send_Message_To_Phone("[ALARM] Gas detected!");
                alarm_count++;
            } else {
                BUZZER_OFF();
            }
        }
        
        // 7.6 短延时
        Delay_ms(10);
    }
}
