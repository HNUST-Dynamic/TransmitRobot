#ifndef ORANGEP_INSTANCE_H
#define ORANGEP_INSTANCE_H

#include "bsp_usart.h"
#include "main.h"
#include "stdbool.h"
#include "stdlib.h"




typedef struct 
{

    UART_HandleTypeDef* usart_handle; // 实例对应的usart_handle
    USARTInstance* usart_instance; // 添加指向 USART 实例的指针

} OrangepiInstance;

extern OrangepiInstance *Orangepi;
extern char command[8];


void Cmd_Callback(USARTInstance* USARTInstance);
void CmdUart_Init();

#endif // ORANGEP_INSTANCE_H
