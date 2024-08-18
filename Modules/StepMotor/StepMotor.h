#ifndef STEPMOTOR_H_
#define STEPMOTOR_H_

#include "bsp_usart.h"
#include "DataFrame.h"
#include "main.h"

#define STEP_MOTOR_CNT 5

// 定义一个函数指针类型，用于电机控制
typedef void (*MotorControlFunction)(struct StepMotorInstance* motor);

/**
 * @brief 步进电机初始化设置结构体
 * 
 */
typedef struct 
{
    uint8_t control_id;      // 控制ID: 默认为0x01
    uint8_t control_mode;    // 控制模式: 0x01=速度控制, 0x02=位置控制, 0x03=力矩控制, 0x04=单圈绝对角度控制
    uint8_t motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint8_t subdivision;     // 细分值: 例如0x20=32细分
    uint8_t data;            // 速度控制模式下为 0；
                             // 位置控制模式下的角度数据，单位为角度;
                             // 力矩控制模式下的电流大小数据，单位为毫安
    uint8_t speed;           // 速度,单位为 rad/s
    UART_HandleTypeDef usart_handle; // 使用具体的 UART_HandleTypeDef 实例
    USARTInstance* usart_instance; // 添加指向 USART 实例的指针
    MotorControlFunction control_function;  // 电机控制函数句柄
} StepMotor_Init_Config_s;

/**
 * @brief 步进电机实例
 * 
 */
typedef struct 
{
    uint8_t control_id;      // 控制ID: 默认为0x01
    uint8_t control_mode;    // 控制模式: 0x01=速度控制, 0x02=位置控制, 0x03=力矩控制, 0x04=单圈绝对角度控制
    uint8_t motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint8_t subdivision;     // 细分值: 例如0x20=32细分
    uint8_t data;            // 速度控制模式下为 0；
                             // 位置控制模式下的角度数据，单位为角度;
                             // 力矩控制模式下的电流大小数据，单位为毫安
    uint8_t speed;           // 速度,单位为 rad/s

    UART_HandleTypeDef usart_handle; // 使用具体的 UART_HandleTypeDef 实例
    USARTInstance* usart_instance; // 添加指向 USART 实例的指针
    MotorControlFunction control_function;  // 电机控制函数句柄
} StepMotorInstance;

StepMotorInstance *StepMotorRegister(StepMotor_Init_Config_s *StepMotor_Init_Config);
void StepMotorControl(StepMotorInstance* motor);

#endif // !STEPMOTOR_H_
