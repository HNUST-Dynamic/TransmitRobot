#ifndef ORANGEP_INSTANCE_H
#define ORANGEP_INSTANCE_H

#include "bsp_usart.h"
#include "main.h"
#include "stdbool.h"
#include "stdlib.h"

typedef struct
{

    UART_HandleTypeDef *usart_handle; // 实例对应的usart_handle
    USARTInstance *usart_instance;    // 添加指向 USART 实例的指针

} OrangepiInstance;

extern OrangepiInstance *Orangepi;
extern volatile uint8_t command[8];
extern volatile uint8_t rangging[6];
extern volatile uint8_t wuliao[24];
extern uint8_t element;
extern int x_int;
extern int y_int;
extern int d;
extern volatile char input_copy[256];
extern char line[16];
extern int num;             

/**
 * @brief 串口dma空闲中断的回调函数
 *
 * @param USARTInstance
 */
void Cmd_Callback(USARTInstance *USARTInstance);

/**
 * @brief 通讯串口初始化
 *
 */
void CmdUart_Init();

/**
 * @brief 检测物料是否稳定
 *
 * @param current_ring
 * @return true
 * @return false
 */
bool IsStable(char current_ring);

/**
 * @brief 检测色环是否稳定
 *
 * @param current_ring
 * @return true
 * @return false
 */
bool Ring_IsStable(char current_ring);

/**
 * @brief 检测当前看到的是否和命令顺序对应上
 *
 * @param current_ring
 * @return true
 * @return false
 */
bool IsMatch(char current_ring);
#endif // ORANGEP_INSTANCE_H
