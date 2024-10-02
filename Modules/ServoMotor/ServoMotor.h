/**
 * @file servo_motor.h
 * @author Libaoen18
 * @brief 舵机控制头文件
 * @version 0.1
 * @date 2024.08.27
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SERVO_MOTOR_H
#define SERVO_MOTOR_H

#include "main.h"
#include "tim.h"
#include <stdint-gcc.h>

#define SERVO_MOTOR_CNT 7



/* 用于初始化不同舵机的结构体,各类舵机通用 */
typedef struct
{
    int16_t angle;
    // 使用的定时器类型及通道
    TIM_HandleTypeDef *htim;
    /*Channel值设定
     *TIM_CHANNEL_1
     *TIM_CHANNEL_2
     *TIM_CHANNEL_3
     *TIM_CHANNEL_4
     *TIM_CHANNEL_ALL
     */
    uint32_t Channel;

} Servo_Init_Config_s;
typedef struct
{
    int16_t angle;
    // 使用的定时器类型及通道
    TIM_HandleTypeDef *htim;
    /*Channel值设定
     *TIM_CHANNEL_1
     *TIM_CHANNEL_2
     *TIM_CHANNEL_3
     *TIM_CHANNEL_4
     *TIM_CHANNEL_ALL
     */
    uint32_t Channel;
} ServoInstance;

ServoInstance *ServoInit(Servo_Init_Config_s *Servo_Init_Config);
void ServoMotor_Init();
void ServoMotor_Control();
void ServoMotor_Set_Angle(ServoInstance* xServo,uint16_t xangle);
#endif // SERVO_MOTOR_H