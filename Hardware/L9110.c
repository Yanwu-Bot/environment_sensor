#include "L9110.h"
#include "Delay.h"

static FanState_TypeDef current_state = FAN_STOP;

/**
 * @brief   风扇初始化
 * @param   无
 * @retval  无
 */
void Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(FAN_INA_CLK | FAN_INB_CLK, ENABLE);
    
    // 配置INA (PA3) 为推挽输出
    GPIO_InitStructure.GPIO_Pin = FAN_INA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FAN_INA_PORT, &GPIO_InitStructure);
    
    // 配置INB (PB0) 为推挽输出
    GPIO_InitStructure.GPIO_Pin = FAN_INB_PIN;
    GPIO_Init(FAN_INB_PORT, &GPIO_InitStructure);
    
    // 初始化为停止状态
    Fan_Stop();
}

/**
 * @brief   风扇控制
 * @param   state: 风扇状态
 * @retval  无
 */
void Fan_Control(FanState_TypeDef state)
{
    switch(state)
    {
        case FAN_STOP:
            // 停止: INA=0, INB=0
            GPIO_ResetBits(FAN_INA_PORT, FAN_INA_PIN);
            GPIO_ResetBits(FAN_INB_PORT, FAN_INB_PIN);
            break;
            
        case FAN_FORWARD:
            // 正转: INA=1, INB=0
            GPIO_SetBits(FAN_INA_PORT, FAN_INA_PIN);
            GPIO_ResetBits(FAN_INB_PORT, FAN_INB_PIN);
            break;
            
        case FAN_REVERSE:
            // 反转: INA=0, INB=1
            GPIO_ResetBits(FAN_INA_PORT, FAN_INA_PIN);
            GPIO_SetBits(FAN_INB_PORT, FAN_INB_PIN);
            break;
            
        case FAN_BRAKE:
            // 刹车: INA=1, INB=1
            GPIO_SetBits(FAN_INA_PORT, FAN_INA_PIN);
            GPIO_SetBits(FAN_INB_PORT, FAN_INB_PIN);
            break;
            
        default:
            return;
    }
    
    current_state = state;
}

/**
 * @brief   正转
 * @param   无
 * @retval  无
 */
void Fan_Forward(void)
{
    Fan_Control(FAN_FORWARD);
}

/**
 * @brief   反转
 * @param   无
 * @retval  无
 */
void Fan_Reverse(void)
{
    Fan_Control(FAN_REVERSE);
}

/**
 * @brief   停止
 * @param   无
 * @retval  无
 */
void Fan_Stop(void)
{
    Fan_Control(FAN_STOP);
}

/**
 * @brief   刹车
 * @param   无
 * @retval  无
 */
void Fan_Brake(void)
{
    Fan_Control(FAN_BRAKE);
}

/**
 * @brief   获取当前风扇状态
 * @param   无
 * @retval  当前状态
 */
FanState_TypeDef Fan_GetState(void)
{
    return current_state;
}

/**
 * @brief   在正转和停止之间切换
 * @param   无
 * @retval  无
 */
void Fan_Toggle(void)
{
    if (current_state == FAN_FORWARD) {
        Fan_Stop();
    } else {
        Fan_Forward();
    }
}
