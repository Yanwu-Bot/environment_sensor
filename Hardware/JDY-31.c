#include "stm32f10x.h"
#include "Delay.h"
#include "LED.h"
#include "JDY-31.h"

uint8_t USART_FLAG = 0;  // 初始化标志变量
uint8_t rx_buffer = 0;   // 接收缓冲区

// 中断函数 - 简化版
void USART3_IRQHandler(void)
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        // 读取数据
        rx_buffer = USART_ReceiveData(USART3);
        
        // 设置标志，但不处理复杂逻辑
        USART_FLAG = rx_buffer;
        
        // 清除中断标志
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}

// 串口初始化 - 简化版
void UART3_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    // TX (PB10) - 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // RX (PB11) - 浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // USART配置
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    
    // NVIC配置 - 使用较低优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  // 较低优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能接收中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    // 使能USART
    USART_Cmd(USART3, ENABLE);
}

// 发送单个字节
void sendbyte(uint16_t Data)  
{
    USART_SendData(USART3, Data);
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

// 处理蓝牙数据（在主循环中调用）
void Process_Bluetooth(void)
{
    static uint8_t last_flag = 0;
    
    if (USART_FLAG != last_flag && USART_FLAG != 0) {
        switch(USART_FLAG) {
            case '1':
            case 1:
                LEDR_ON();
                LEDG_OFF();
                LEDB_OFF();
                sendbyte('R');  // 回复
                break;
            case '2':
            case 2:
                LEDR_OFF();
                LEDG_ON();
                LEDB_OFF();
                sendbyte('G');
                break;
            case '3':
            case 3:
                LEDR_OFF();
                LEDG_OFF();
                LEDB_ON();
                sendbyte('B');
                break;
            default:
                // 未知命令
                break;
        }
        last_flag = USART_FLAG;
        USART_FLAG = 0;  // 清空标志
    }
}

// 在 JDY-31.c 中修改
void Send_Message_To_Phone(const char* message)
{
    // 添加超时保护
    uint32_t timeout;
    
    while(*message) {
        timeout = 10000;  // 超时计数
        USART_SendData(USART3, (uint16_t)(*message));
        
        // 等待发送完成，但有超时保护
        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) {
            if(--timeout == 0) break;  // 超时退出
        }
        
        message++;
    }
    
    // 发送换行符
    timeout = 10000;
    USART_SendData(USART3, '\r');
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) {
        if(--timeout == 0) return;
    }
    
    timeout = 10000;
    USART_SendData(USART3, '\n');
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) {
        if(--timeout == 0) return;
    }
}