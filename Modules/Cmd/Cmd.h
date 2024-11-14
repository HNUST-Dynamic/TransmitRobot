#ifndef ORANGEP_INSTANCE_H
#define ORANGEP_INSTANCE_H

#include "bsp_usart.h"
#include "main.h"
#include "stdbool.h"
#include "stdlib.h"




typedef struct 
{

    UART_HandleTypeDef* usart_handle; // 实例对应的usart_handle
    USARTInstance* usart_instance; // 添加指向 USART 实例的指�?

} OrangepiInstance;

extern OrangepiInstance *Orangepi;
extern volatile uint8_t command[8];
extern uint8_t r_command[8];
extern volatile uint8_t rangging[6];
extern volatile uint8_t wuliao[24];
extern bool IsMatch();
extern uint8_t element;
extern int x_int;
extern int y_int;
extern int d;

void Cmd_Callback(USARTInstance* USARTInstance);
void CmdUart_Init();
bool IsStable();
bool Ring_IsStable();
bool IsMatch();
bool IsAccurate();
int yuanpan_error();
int sehuan_error();
int maduo_error();
int x_error();
int y_error();
int rang_error();
#endif // ORANGEP_INSTANCE_H
