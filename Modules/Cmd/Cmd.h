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
extern uint8_t command[8];
extern uint8_t r_command[8];
extern uint8_t wuliao[16];
extern bool IsMatch();
extern uint8_t element;
extern int error_x ,error_y;

void Cmd_Callback(USARTInstance* USARTInstance);
void CmdUart_Init();
bool IsStable();
bool IsMatch();
bool IsAccurate();
int yuanpan_error();
int sehuan_error();
int maduo_error();

#endif // ORANGEP_INSTANCE_H
