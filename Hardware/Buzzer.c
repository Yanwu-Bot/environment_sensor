#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "Buzzer.h"


void BUZZER_Init()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	// GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	BUZZER_OFF();  
	
}   

void BUZZER_ON(void)                   //pin0�õ͵�ƽ
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);

}

void BUZZER_OFF(void)                   //pin0�øߵ�ƽ
{
	GPIO_SetBits(GPIOA,GPIO_Pin_2);

}
void BUZZER_Turn(void)                   //ת���ߵ͵�ƽ
{
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}

void BUZZER_50ms(void)
{
	BUZZER_ON();
	Delay_ms(50);
	BUZZER_OFF();
}
