#ifndef __JDY_31_H
#define __JDY_31_H

#include <stdint.h>   // ?? <> ??
#include <stdio.h>    // ?? FILE ??


extern uint8_t USART_FLAG;  // ????.c????,?????extern


void UART3_init(void);
void sendbyte(uint16_t Data);
int fputc(int ch, FILE *f);
void Run_JDY_31(void);
void Send_Message_To_Phone(const char* message);

#endif
