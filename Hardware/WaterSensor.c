#include "WaterSensor.h"                 //水传感器
#include "stm32f10x.h"                  // Device header

void Water_Init(void)
{
	GPIO_InitTypeDef GPIO_InitTypeStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;

	GPIO_InitTypeStructure.GPIO_Mode = GPIO_Mode_IPU;   //下拉输入
	GPIO_InitTypeStructure.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitTypeStructure);
}


int Item_detection(void)  //检测到信号，低电平为检测到
{
	uint16_t temp;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5) == 0)
	{
		temp = 1;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6) == 0)
	{
		temp = 2;
	}
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7) == 0)
	{
		temp = 3;
	}
	return temp;
}
